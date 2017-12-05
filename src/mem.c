/*
 * mem.c
 *	基本思路为16个chunk,每个chunk下有32个slot,每个slot的大小由slot_size_array确定
 *	sizeof(jvm_obj)=32,所以大小从32开始
 *	目前先支持小对象的分配，大对象分配后续实现
 *	申请内存时，首先判断当前chunk是否需要进行回收
 *	如果当前没有可用的slot，首先merge被借出去的slot,如果还没可用内存,向后面的chunk借
 *
 *      Author: chengzhang
 */


#include "mem.h"
#include <stdlib.h>
#include "czvm.h"


unsigned int slot_size_array[] = {32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576};


jvm_mem* init_jvm_mem()
{
	/**
	 * 1,init jvm_mem
	 */
	jvm_mem* mem = (jvm_mem*)malloc(sizeof(jvm_mem));
	mem->chunk_array_size = CHUNK_ARRAY_SIZE;
	mem->total_allocted = 0;
	mem->total_used = 0;
	mem->chunk_array = (mem_chunk*)malloc(sizeof(mem_chunk)*CHUNK_ARRAY_SIZE);

	/**
	 * 2,init mem_chunk
	 */
	int i;
	for(i=0;i< mem->chunk_array_size;i++)
	{
		mem_chunk* chunk = mem->chunk_array+i;
		chunk->slots_array_num = SLOT_ARRAY_SIZE_PER_CHUNK;
		chunk->per_slot_size = slot_size_array[i];
		chunk->used_slots = NULL;
		chunk->empty_slots = NULL;
		chunk->empty_size = 0;
		chunk->used_size = 0;

		int j;
		for(j=0;j<chunk->slots_array_num;j++)
		{
			chunk_slot* cs = (chunk_slot*)malloc(sizeof(chunk_slot));
			cs->slot_length = chunk->per_slot_size;
			cs->next = NULL;
			cs->star = (unsigned char*)malloc(sizeof(unsigned char)*cs->slot_length);
			cs->end = cs->star + cs->slot_length-1;
			cs->chunk_index = i;
			cs->slot_index_in_chunk = j;
			chunk->empty_size += cs->slot_length;

			mem->total_allocted += cs->slot_length;

			cs->next = chunk->empty_slots;
			chunk->empty_slots = cs;
		}
	}

	return mem;

}

chunk_slot* remove_slot(chunk_slot* head,chunk_slot* need_remove)
{
	if(head == need_remove)
	{
		head = need_remove->next;
	}
	else
	{
		while(head->next != need_remove)
		{
			head = head->next;
		}
		head->next = need_remove->next;
	}

	return head;
}

chunk_slot* insert_before_head(chunk_slot* head,chunk_slot* need_insert)
{
	need_insert->next = head;
	head = need_insert;
	return head;
}

chunk_slot* insert_after_tail(chunk_slot* head,chunk_slot* need_insert)
{
	chunk_slot* p = head;
	if(NULL == p){
		head = need_insert;
	}else{
		while( NULL != p->next)
		{
			p = p->next;
		}
		p->next = need_insert;
	}
	return head;
}

void* mem_alloc(jvm_mem* mem , size_t size)
{
	//consider small memory first

	/**
	 * find match slot size
	 */
	int i;
	for(i=0;i<sizeof(slot_size_array)/sizeof(int);i++)
	{
		if(size > slot_size_array[i])
			continue;
		break;
	}

	mem_chunk* chunk = mem->chunk_array+i;

	/**
	 * need gc?
	 */
	if(chunk->empty_size==0 || ((double)chunk->used_size/((double)chunk->empty_size + chunk->used_size) >= CHUNK_GC_FACTOR))
	{
		chunk_slot* u_slot = chunk->used_slots;
		chunk_slot* pre_uslot = u_slot;

		while(u_slot)
		{
			chunk_slot *s = u_slot->next;
			jvm_obj* obj = (jvm_obj*)(u_slot->star);
			if(0 == obj->ref_count)
			{
				if(u_slot == pre_uslot)
				{
					//head
					chunk->used_slots = u_slot->next;
				}
				else
				{
					pre_uslot->next = u_slot->next;
				}
				u_slot->next = NULL;
				/*u_slot->next = chunk->empty_slots;
				chunk->empty_slots = u_slot;*/
				if(mem->chunk_array+u_slot->chunk_index == chunk)
				{
					chunk->empty_slots = insert_before_head(chunk->empty_slots,u_slot);
				}
				else
				{
					chunk->empty_slots = insert_after_tail(chunk->empty_slots,u_slot);
				}
				chunk->empty_size += u_slot->slot_length;
				chunk->used_size -= u_slot->slot_length;
			}

			pre_uslot = u_slot;
			u_slot = s;

		}

	}

	chunk_slot* slot = chunk->empty_slots;
	if(!slot)
	{
		//need merge other slots? todo needcheck and optimize
		int j = i-1;
		while(j>=0)
		{
			chunk_slot* temp_slot[256] = {NULL};
			int idx = 0;
			mem_chunk * ck = mem->chunk_array+j;
			chunk_slot* e_slot = ck->empty_slots;
			int size = e_slot->slot_length;
			int index = e_slot->slot_index_in_chunk;
			temp_slot[idx++] = e_slot;
			chunk_slot* next_slot = e_slot->next;
			while(next_slot)
			{
				if(idx==256)
					break;
				if(size == chunk->per_slot_size)
					break;
				if(next_slot->slot_index_in_chunk == index)
				{
					temp_slot[idx++] = next_slot;
					size += next_slot->slot_length;
				}
				next_slot = next_slot->next;
			}
			if(size == chunk->per_slot_size)
			{
				int d = 0;
				while(temp_slot[d] != NULL)
				{
					ck->empty_slots = remove_slot(ck->empty_slots,temp_slot[d]);
					d++;
				}
				d = 1;
				long addr = temp_slot[0]->star;
				int id = 0;
				while(temp_slot[d]!=NULL)
				{
					if(temp_slot[d]->star < addr)
					{
						id = d;
						addr = temp_slot[d]-> star;
					}
					d++;
				}
				chunk_slot* s = temp_slot[id];
				s->slot_length = chunk->per_slot_size;
				s->end = s->star+s->slot_length-1;
				s->next = chunk->empty_slots;
				chunk->empty_slots = s;
				slot = chunk->empty_slots;
				chunk->empty_size += s->slot_length;

				d = 0;
				while(temp_slot[d]!=NULL)
				{
					if(d!=id)
					{
						free(temp_slot[d]);
					}
					d++;
				}
				break;
			}
			j--;

		}
	}

	if(!slot)
	{
		//need borrow from big slot
		int ii = i+1;
		mem_chunk* ck = mem->chunk_array+ii;
		while(ck->empty_size == 0 && ii<CHUNK_ARRAY_SIZE)
		{
			ii++;
			ck = mem->chunk_array+ii;
		}
		if(ii == CHUNK_ARRAY_SIZE)
		{
			return NULL;
		}

		int count = ck->per_slot_size/chunk->per_slot_size;
		chunk_slot* t_slot = ck->empty_slots;
		ck->empty_slots = t_slot->next;
		ck->empty_size -= ck->per_slot_size;

		int jj = 0;
		for(jj=0;jj<count;jj++)
		{
			chunk_slot* st = (chunk_slot*)malloc(sizeof(chunk_slot));
			st->chunk_index = t_slot->chunk_index;
			st->next = NULL;
			st->slot_index_in_chunk = t_slot->slot_index_in_chunk;
			st->slot_length = chunk->per_slot_size;
			st->star = t_slot->star+jj*st->slot_length;
			st->end = st->star+st->slot_length-1;
			st->next = chunk->empty_slots;
			chunk->empty_slots = st;
		}
		free(t_slot);

		slot = chunk->empty_slots;

	}

	void* addr = (void*)(slot->star);
	if(mem->chunk_array+slot->chunk_index == chunk)
	{
		chunk->empty_size -= slot->slot_length;
		chunk->used_size += slot->slot_length;
	}
	chunk->empty_slots = slot->next;
	slot->next = chunk->used_slots;
	chunk->used_slots = slot;
	return addr;
}

/*int main(int argc,char** argv)
{
	jvm_mem* mem = init_jvm_mem();
	char* c1 = (char*)mem_alloc(mem,32);
	jvm_obj* o1 = (jvm_obj*)c1;
	o1->ref_count = 1;
	char* c2 = (char*)mem_alloc(mem,32);
	jvm_obj* o2 = (jvm_obj*)c2;
	o2->ref_count = 1;

	char* c3 = mem_alloc(mem,64);
	jvm_obj* o3 = (jvm_obj*)c3;
	o3->ref_count = 1;
	o1->ref_count = 0;
	o2->ref_count = 0;

	char* c4 = mem_alloc(mem,64);
	jvm_obj* o4 = (jvm_obj*)c4;
	o4->ref_count = 1;
	char* c5 = mem_alloc(mem,32);

	char* c6 = mem_alloc(mem,64);

	return 0;
}*/
