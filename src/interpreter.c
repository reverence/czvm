/*
 * interpreter.c
 *
 *
 *      Author: chengzhang
 */

#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytecode.h"
#include "czvm.h"
#include "list.h"
#include "log.h"
#include "type.h"
#include "mem.h"

jvm_stack_obj_list* add_object_to_cur_stack_list(jvm_stack_frame* cur_stack_frame,jvm_obj* obj);

jvm_stack_obj_list* remove_jvm_object_from_stack_list(jvm_stack_frame* frame,jvm_obj* obj);

int interpreter_byte_code(class_method* method);

class_method* get_class_method(u2 method_index);

int get_arg_size(char* desc_name);

long* get_invoke_args(int arg_size);



#define push_operand_stack(type,value)	\
	do{	\
		*(type*)(cur_stack_frame->operand_stacks+cur_stack_frame->operand_offset) = (type)value;				\
		cur_stack_frame->operand_offset += STACK_ITEM_SIZE;	\
		print_frame_info(cur_stack_frame);	\
	}while(0);

#define pop_operand_stack(type, value)	\
		do{		\
			cur_stack_frame->operand_offset -= STACK_ITEM_SIZE;	\
			value = *(type*)(cur_stack_frame->operand_stacks+cur_stack_frame->operand_offset);	\
			*(type*)(cur_stack_frame->operand_stacks+cur_stack_frame->operand_offset) = 0;	\
			print_frame_info(cur_stack_frame);	\
		}while(0);

#define set_local_table(type, index, value)						\
	do {										\
		*(type *)(cur_stack_frame->local_variables + index * STACK_ITEM_SIZE) = value;\
		print_frame_info(cur_stack_frame);					\
	} while(0);

#define jvm_store_data(type,index)	\
		do{		\
			long tmp;	\
			pop_operand_stack(type,tmp);	\
			set_local_table(type,index,tmp);	\
			print_frame_info(cur_stack_frame);	\
		}while(0);

#define get_local_table(value, type, index)						\
	do {										\
		value = *(type *)(cur_stack_frame->local_variables + index * STACK_ITEM_SIZE);\
		print_frame_info(cur_stack_frame);						\
	} while(0);

#define jvm_load_data(type,index)	\
		do{		\
			long temp;	\
			get_local_table(temp,long,index);	\
			push_operand_stack(long,temp);	\
		}while(0);

void print_frame_info(jvm_stack_frame* cur_jvm_stack_frame)
{
	int i;
	char buf[1024];
	memset(buf,'\0',1024);
	char *p = buf;
	sprintf(buf,"%s","local variables:");
	p+=strlen(buf);
	for(i=0;i<cur_jvm_stack_frame->max_locals;i++)
	{
		sprintf(p,"0x%lx ",*(long*)(cur_jvm_stack_frame->local_variables+i*STACK_ITEM_SIZE));
		p += strlen(p);

	}
	char log[2048] = {'\0'};
	sprintf(log,"%s %s %s   %s\n",__DATE__,__TIME__,__FILE__,buf);
	write_log(log);
	memset(buf,'\0',1024);
	sprintf(buf,"%s","operand stacks:");
	p = buf;
	p+=strlen(buf);
	for(i=0;i<cur_jvm_stack_frame->max_stack;i++)
	{
		sprintf(p,"0x%lx ",*(long*)(cur_jvm_stack_frame->operand_stacks+i*STACK_ITEM_SIZE));
		p += strlen(p);
	}
	memset(log,'\0',2048);
	sprintf(log,"%s %s %s   %s\n",__DATE__,__TIME__,__FILE__,buf);
	write_log(log);

}

void print_interp_info(char* code_name){
	char buf[256];
	memset(buf,'\0',256);
	sprintf(buf,"%s %s %s   exec jvm code:%s\n",__DATE__,__TIME__,__FILE__,code_name);
	write_log(buf);
}

int get_arg_size(char* desc_name)
{
	int arg_size = 0;
	while(*desc_name != ')')
	{
		if(*desc_name=='I' || *desc_name=='J' || *desc_name=='F' || *desc_name=='D' || *desc_name=='C' || *desc_name=='Z')
		{
			arg_size++;
		}
		if(*desc_name == 'L')
		{
			while(*desc_name!=';')
			{
				desc_name++;
			}
			arg_size++;
		}
		desc_name++;
	}

	return arg_size;
}

int jvm_interp_nop(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aconst_null(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_m1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_4(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,4);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iconst_5(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(int,5);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lconst_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lconst_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fconst_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(float,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fconst_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(float,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fconst_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(float,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dconst_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(double,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dconst_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	push_operand_stack(double,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_bipush(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value = (int)(*(u1*)(info+1));
	push_operand_stack(long,value);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_sipush(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value = (int)(*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	push_operand_stack(short,value);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_ldc(u2 len, char *code_name, void *info)
{
	//int float or string
	print_interp_info(code_name);
	u2 index;
	index = (u2)((*(u1 *)(info + 1)));

	u1 tag = cur_interpreter_env->cp_info[index].tag;
	u2 string_index;
	int value;
	char* str;
	switch(tag)
	{
	case CONSTANT_String:
		string_index = ((constant_string_info*)(cur_interpreter_env->cp_info[index].info))->string_index;
		str = ((constant_utf8_info*)(cur_interpreter_env->cp_info[string_index].info))->bytes;
		jvm_obj* obj = mem_alloc(jvm_mem_addr,sizeof(jvm_obj));
		if(!obj)
		{
			exit(1);
		}
		obj->ref_count = 0;
		obj->value = str;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,obj);
		push_operand_stack(long,obj);
		break;
	case CONSTANT_Integer:
		value = ((constant_integer_info*)(cur_interpreter_env->cp_info[index].info))->bytes;
		push_operand_stack(long,value);
		break;
	case CONSTANT_Float:
		value = ((constant_float_info*)(cur_interpreter_env->cp_info[index].info))->bytes;
		push_operand_stack(long,value);
		break;
	}


	vm_pc.pc += len;
	return 0;
}
int jvm_interp_ldc_w(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ldc2_w(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long high_bytes, low_bytes;
	long value = 0;
	u2 index;
	index = (u2)(((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2)));
	high_bytes = ((constant_long_info*)(cur_interpreter_env->cp_info[index].info))->high_bytes;
	low_bytes = ((constant_long_info*)(cur_interpreter_env->cp_info[index].info))->low_bytes;
	value = ((high_bytes << 32) & 0xffffffff00000000) |
				(low_bytes & 0x00000000ffffffff);
	push_operand_stack(long,value);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_load_data(int,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_load_data(long,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	u1 index;
	index = *(u1 *)(info + 1);
	jvm_load_data(long,index);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_load_data(long,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_load_data(long,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iload_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	print_interp_info(code_name);
	jvm_load_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iload_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iload_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_iload_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lload_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lload_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lload_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lload_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fload_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fload_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fload_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fload_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dload_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dload_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dload_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dload_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_aload_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_aload_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,1);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aload_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,2);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aload_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_load_data(long,3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iaload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index,*addr,value;
	pop_operand_stack(int, index)
	pop_operand_stack(int, addr)
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (int)(obj->value);
	push_operand_stack(int,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_laload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index;
	long value,*addr;
	pop_operand_stack(int,index)
	pop_operand_stack(long,addr)
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (long)(obj->value);
	push_operand_stack(long,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_faload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index;
	float value,*addr;
	pop_operand_stack(int,index)
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (float)(obj->value);
	push_operand_stack(float,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_daload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index;
	double value,*addr;
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (double)(obj->value);
	push_operand_stack(double,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aaload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index;
	long* addr;
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	push_operand_stack(long,obj);

		vm_pc.pc += len;
	return 0;
}
int jvm_interp_baload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index,*addr,value;
	pop_operand_stack(int, index)
	pop_operand_stack(int, addr)

	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (int)(obj->value);
	push_operand_stack(int,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_caload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index,*addr,value;
	pop_operand_stack(int, index)
	pop_operand_stack(int, addr)

	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (int)(obj->value);
	push_operand_stack(int,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_saload(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int index,*addr,value;
	pop_operand_stack(int, index)
	pop_operand_stack(int, addr)

	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	value = (int)(obj->value);
	push_operand_stack(int,value);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_istore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_store_data(long,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lstore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
		index = *(u1 *)(info + 1);
		jvm_store_data(long,index);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fstore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
	index = *(u1 *)(info + 1);
	jvm_store_data(long,index);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dstore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
	index = *(u1 *)(info + 1);
	jvm_store_data(long,index);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_astore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u1 index;
	index = *(u1 *)(info + 1);
	jvm_store_data(long,index);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_istore_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(int,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_istore_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(int,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_istore_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(int,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_istore_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(int,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lstore_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,0);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lstore_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,1);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lstore_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,2);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lstore_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fstore_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,0);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fstore_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,1);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fstore_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,2);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fstore_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dstore_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,0);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dstore_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,1);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dstore_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,2);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dstore_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_astore_0(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,0);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_astore_1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,1);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_astore_2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,2);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_astore_3(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	jvm_store_data(long,3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value,index,*addr;
	pop_operand_stack(int,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long value,index,*addr;
	pop_operand_stack(long,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float value;
	int index;
	float* addr;
	pop_operand_stack(float,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double value,*addr;
	int index;
	pop_operand_stack(double,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_aastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	long value,*addr;
	int index;
	pop_operand_stack(long,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_bastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value,index,*addr;
	pop_operand_stack(int,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_castore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value,index,*addr;
	pop_operand_stack(int,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_sastore(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int value,index,*addr;
	pop_operand_stack(int,value);
	pop_operand_stack(int,index);
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj = ((jvm_obj*)(obj->value)+index);
	obj->value = value;
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_pop(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_pop2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long tmp;
	pop_operand_stack(long,tmp);
	push_operand_stack(long,tmp);
	push_operand_stack(long,tmp);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup_x1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long tmp;
		pop_operand_stack(long,tmp);
		push_operand_stack(long,tmp);
		push_operand_stack(long,tmp);
		push_operand_stack(long,tmp);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup_x2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup2_x1(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dup2_x2(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_swap(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iadd(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int l1;
	int l2;
	pop_operand_stack(int,l1);
	pop_operand_stack(int,l2);
	push_operand_stack(int,(l1+l2));
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_ladd(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long l1;
	long l2;
	pop_operand_stack(long,l1);
	pop_operand_stack(long,l2);
	push_operand_stack(long,(l1+l2));
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fadd(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float f1;
	float f2;
	pop_operand_stack(float,f1);
	pop_operand_stack(float,f2);

	float f = f1+f2;
	push_operand_stack(float,f);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dadd(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	double dd1;
	double dd2;
	pop_operand_stack(double,dd1);
	pop_operand_stack(double,dd2);
	push_operand_stack(double,(dd1+dd2));
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_isub(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int a,b;
	pop_operand_stack(int,a);
	pop_operand_stack(int,b);
	int c = b-a;
	push_operand_stack(int,c);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lsub(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long l1,l2;
	pop_operand_stack(long,l1);
	pop_operand_stack(long,l2);
	long l3 = l2 - l1;
	push_operand_stack(long,l3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fsub(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float f1,f2;
	pop_operand_stack(float,f1);
	pop_operand_stack(float,f2);
	float f3 = f2 - f1;
	push_operand_stack(float,f3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dsub(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double d1,d2;
	pop_operand_stack(double,d1);
	pop_operand_stack(double,d2);
	double d3 = d2 - d1;
	push_operand_stack(double,d3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_imul(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int a,b;
	pop_operand_stack(int,a);
	pop_operand_stack(int,b);
	int c = a*b;
	push_operand_stack(int,c);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_lmul(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long a,b;
	pop_operand_stack(long,a);
	pop_operand_stack(long,b);
	long c = a*b;
	push_operand_stack(long,c);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_fmul(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float f1,f2;
	pop_operand_stack(float,f1);
	pop_operand_stack(float,f2);
	float f3 = f1*f2;
	push_operand_stack(float,f3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_dmul(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double d1,d2;
	pop_operand_stack(double,d1);
	pop_operand_stack(double,d2);
	double d3 = d1*d2;
	push_operand_stack(double,d3);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_idiv(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int a,b;
	pop_operand_stack(int,a);
	pop_operand_stack(int,b);
	int c = b/a;
	push_operand_stack(int,c);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ldiv(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long a,b;
	pop_operand_stack(long,a);
	pop_operand_stack(long,b);
	long c = b/a;
	push_operand_stack(long,c);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fdiv(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float f1,f2;
	pop_operand_stack(float,f1);
	pop_operand_stack(float,f2);
	float f3 = f2/f1;
	push_operand_stack(float,f3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ddiv(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double d1,d2;
	pop_operand_stack(double,d1);
	pop_operand_stack(double,d2);
	double d3 = d2/d1;
	push_operand_stack(double,d3);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_irem(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lrem(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_frem(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_drem(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ineg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lneg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fneg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dneg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ishl(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lshl(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ishr(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lshr(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iushr(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lushr(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iand(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_land(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ior(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lor(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ixor(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lxor(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iinc(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2l(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2f(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int tmp;
	pop_operand_stack(int,tmp);
	//long l = int2float(tmp);
	//push_operand_stack(long,l);
	float f = (float)tmp;
	push_operand_stack(float,f);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2d(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int tmp;
	pop_operand_stack(int,tmp);
	double d = (double)tmp;
	push_operand_stack(double,d);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_l2i(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long tmp;
	pop_operand_stack(long,tmp);
	push_operand_stack(int,tmp);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_l2f(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long tmp;
	pop_operand_stack(long,tmp);
	float f = (float)tmp;
	push_operand_stack(float,f);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_l2d(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	long tmp;
	pop_operand_stack(long,tmp);
	double b = (double)tmp;
	push_operand_stack(double,b);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_f2i(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float tmp;
	pop_operand_stack(float,tmp);
	int a = (int)tmp;
	push_operand_stack(int,a);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_f2l(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float tmp;
	pop_operand_stack(float,tmp);
	long a = (long)tmp;
	push_operand_stack(long,a);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_f2d(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	float tmp;
	pop_operand_stack(float,tmp);
	double d = (double)tmp;
	push_operand_stack(double,d);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_d2i(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double tmp;
	pop_operand_stack(double,tmp);
	int a = (int)tmp;
	push_operand_stack(int,a);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_d2l(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double tmp;
	pop_operand_stack(double,tmp);
	long a = (long)tmp;
	push_operand_stack(long,a);
	vm_pc.pc += len;
	return 0;
}
int jvm_interp_d2f(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	double tmp;
	pop_operand_stack(double,tmp);
	float f = (float)tmp;
	push_operand_stack(float,f);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2b(u2 len, char *code_name, void *info)
{
	//TODO
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2c(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int tmp;
	pop_operand_stack(int,tmp);
	char a = (char)tmp;
	push_operand_stack(char,a);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_i2s(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int tmp;
	pop_operand_stack(int,tmp);
	short a = (short)tmp;
	push_operand_stack(short,a);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lcmp(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fcmpl(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_fcmpg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dcmpl(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dcmpg(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifeq(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int temp;
	pop_operand_stack(int,temp)
	if(0 == temp)
	{
		u2 index;
		index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
		vm_pc.pc += index;
		return 0;
	}
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifne(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_iflt(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifge(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifgt(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifle(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmpeq(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmpne(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmplt(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmpge(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmpgt(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_icmple(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_acmpeq(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_if_acmpne(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_goto(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));

	vm_pc.pc += index;
	return 0;
}
int jvm_interp_jsr(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ret(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_tableswitch(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lookupswitch(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ireturn(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_lreturn(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_freturn(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_dreturn(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_areturn(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}

jvm_stack_obj_list* remove_jvm_object_from_stack_list(jvm_stack_frame* frame,jvm_obj* obj)
{

	//process field

	if(NULL != obj->field_values)
	{
		obj_field_value* f_value = obj->field_values;
		while(f_value)
		{
			if(!is_primitive_desc_type(f_value->field_desc))
			{
				jvm_obj* o = (jvm_obj*)(f_value->value);
				if(o->ref_count>0)
					o->ref_count--;
			}
			f_value = f_value->next;
		}
	}

	jvm_stack_obj_list * list = frame->list;
	if(NULL != list)
	{
		if(list->obj == obj)
		{
			jvm_stack_obj_list* t = list;
			list = t->next;
			if(((jvm_obj*)(t->obj))->ref_count > 0)
			{
				((jvm_obj*)(t->obj))->ref_count--;
			}
			free(t);
		}
		else
		{
			jvm_stack_obj_list* p1 = list;
			while(NULL != p1)
			{
				if(NULL != p1->next && p1->next->obj == obj)
					break;
				p1 = p1->next;
			}
			if(NULL != p1)
			{
				jvm_stack_obj_list* tmp = p1->next;
				if(((jvm_obj*)(tmp->obj))->ref_count >0 )
				{
					((jvm_obj*)(tmp->obj))->ref_count--;
				}
				p1->next = tmp->next;
				free(tmp);
			}
		}
	}

	return list;
}

void remove_object_from_stack_list(jvm_stack_obj_list* list)
{
	if(NULL != list)
	{
		jvm_stack_obj_list* p1;
		p1 = list;
		jvm_stack_obj_list* p2 = list->next;

		while(NULL != p1)
		{
			jvm_obj* o = (jvm_obj*)(p1->obj);
			if(o->ref_count > 0)
			{
				o->ref_count--;

				//process field
				obj_field_value* v = o->field_values;
				while(v)
				{
					if(!is_primitive_desc_type(v->field_desc))
					{
						if(((jvm_obj*)(v->value))->ref_count > 0)
							((jvm_obj*)(v->value))->ref_count--;
					}
					v = v->next;
				}
			}
			free(p1);
			if(NULL == p2)
				break;
			p1 = p2;
			p2 = p2->next;
		}
	}

}

int jvm_interp_return(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	remove_object_from_stack_list(cur_stack_frame->list);
	cur_stack_frame->list = NULL;

	u1* return_addr;
	return_addr = cur_stack_frame->return_address;
	if(cur_stack_frame->prev)
	{
		jvm_stack_frame* tmp_frame;
		tmp_frame = cur_stack_frame;
		cur_stack_frame = cur_stack_frame->prev;
		//free(tmp_frame);
	}

	if(cur_interpreter_env->prev)
	{
		interpreter_env* tmp_env;
		tmp_env = cur_interpreter_env;
		cur_interpreter_env = tmp_env->prev;
		//free(tmp_env);
	}

	jvm_stack_depth--;
	if (jvm_stack_depth == 0)
	{
		vm_pc.pc += len;
	}
	else
	{
		vm_pc.pc = return_addr;
	}
	return 0;
}
int jvm_interp_getstatic(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 class_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->class_index;
	u2 class_name_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_name_index].info))->bytes;
	u2 desc_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;
	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->name_index;
	desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->descriptor_index;
	char* field_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;
	class_field* field = find_class_field(&class_list_head,class_name,field_name);

	if(!field)
	{
		goto out;//TODO waiting modify
	}
	/*char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;
	if(0 == strcmp("Ljava/lang/String;",desc_name))
	{

	}*/

	push_operand_stack(long,(field->field_value));

	//TODO how to deal it?
out:	vm_pc.pc += len;
	return 0;
}
int jvm_interp_putstatic(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 class_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->class_index;
	class_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_index].info))->bytes;
	u2 desc_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;
	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->name_index;
	desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->descriptor_index;
	char* field_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;

	class_field* field = find_class_field(&class_list_head,class_name,field_name);
	if(!field)
	{
		goto out;//directly out TODO waiting for modify
	}

	char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;

	long l;
	pop_operand_stack(long,l);

	long value = field->field_value;
	if(0 != value)
	{
		if((0 != strcmp(desc_name,"I")) &&  (0 != strcmp(desc_name,"J")) && (0 != strcmp(desc_name,"D")) && (0 != strcmp(desc_name,"C")) && (0 != strcmp(desc_name,"Z"))
				&& (0 != strcmp(desc_name,"F")))
		{
			jvm_obj* o = (jvm_obj*)value;
			while(o)
			{
				if(o->ref_count>0)
				{
					o->ref_count--;
				}
				o = (jvm_obj*)(o->value);
			}
		}
	}

	field->field_value = l;

	if((0 != strcmp(desc_name,"I")) &&  (0 != strcmp(desc_name,"J")) && (0 != strcmp(desc_name,"D")) && (0 != strcmp(desc_name,"C")) && (0 != strcmp(desc_name,"Z"))
					&& (0 != strcmp(desc_name,"F")))
	{
		jvm_obj* obj = (jvm_obj*)l;
		if(obj)
		{
			//remove obj from stack frame
			jvm_obj* p = obj;
			while(NULL != p)
			{
				cur_stack_frame->list = remove_jvm_object_from_stack_list(cur_stack_frame,p);
				if(p->isArray == 1)
				{
					p = (jvm_obj*)(p->value);
				}
				else
				{
					p = NULL;
				}

			}

			p = obj;
			while(NULL != p)
			{
				p->ref_count++;
				if(p->isArray == 1)
				{
					p = (jvm_obj*)(p->value);
				}
				else
				{
					p = NULL;
				}

			}

		}
	}

out:	vm_pc.pc += len;

	return 0;
}

long* get_invoke_args(int arg_size)
{
	long* value = NULL;
	if(arg_size>0)
	{
		value = (long*)malloc(sizeof(long)*arg_size);
		if(!value)
			exit(1);

		int tmp = arg_size;
		while(tmp>0)
		{
			pop_operand_stack(long,(*(value+tmp-1)));
			tmp--;
		}
	}
	return value;
}

jvm_stack_frame* copy_stack_frame(jvm_stack_frame* cur_stack_frame)
{
	jvm_stack_frame* pre_stack_frame = (jvm_stack_frame*)malloc(sizeof(jvm_stack_frame));
	if(!pre_stack_frame)
		exit(1);
	memset(pre_stack_frame,'\0',sizeof(jvm_stack_frame));
	memcpy(pre_stack_frame,cur_stack_frame,sizeof(jvm_stack_frame));

	return pre_stack_frame;
}

interpreter_env* copy_interp_env(interpreter_env* cur_interpreter_env)
{
	interpreter_env* pre_env = (interpreter_env*)malloc(sizeof(interpreter_env));
	if(!pre_env)
		exit(1);
	memset(pre_env,'\0',sizeof(interpreter_env));
	memcpy(pre_env,cur_interpreter_env,sizeof(interpreter_env));

	return pre_env;
}

int jvm_interp_invokevirtual(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));

	u2 class_index = ((constant_method_info*)(cur_interpreter_env->cp_info[index].info))->class_index;
	u2 class_name_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_name_index].info))->bytes;
	u2 desc_index = ((constant_method_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;
	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->name_index;
	desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->descriptor_index;
	char* method_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;
	char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;

	class_method* method;
	if(0==strcmp("println",method_name) || 0==strcmp("print",method_name))// only support print???
	{
		if(0 == strcmp("(Ljava/lang/String;)V",desc_name))
		{
			long addr;
			pop_operand_stack(long,addr);
			jvm_obj* obj = (jvm_obj*)addr;
			char* tmp = (char*)(obj->value);
			printf("%s\n",tmp);
			goto out;
		}else if(0 == strcmp("(F)V",desc_name))
		{
			float fl;
			pop_operand_stack(float,fl);
			printf("%f\n",fl);
			goto out;
		}else if(0 == strcmp("(D)V",desc_name))
		{
			double d;
			pop_operand_stack(double,d);
			printf("%g\n",d);
			goto out;
		}else if(0 == strcmp("(I)V",desc_name))
		{
			long l;
			pop_operand_stack(long,l);
			printf("%d\n",l);
			goto out;
		}else if(0 == strcmp("(J)V",desc_name))
		{
			long l;
			pop_operand_stack(long,l);
			printf("%ld\n",l);
			goto out;
		}else if(0 == strcmp("(C)V",desc_name))
		{
			char c;
			pop_operand_stack(char,c);
			printf("%c\n",c);
			goto out;
		}else if(0 == strcmp("(Z)V",desc_name))
		{
			int i;
			pop_operand_stack(int,i);
			if(i == 1){
				printf("%s\n","true");
			}else{
				printf("%s\n","false");
			}
			goto out;
		}
	}else
	{
		method = find_class_method(&class_list_head,class_name,method_name,desc_name);
		if(!method)
		{
			CLASS* newClass = jvm_load_class(jvmargs->classpath,class_name);
			if(!newClass)
			{
				goto out;
			}
			method = find_class_method(&class_list_head,class_name,method_name,desc_name);
		}

		/**
		 * get args
		 */
		int arg_size = get_arg_size(desc_name);
		long* values = get_invoke_args(arg_size);

		long value;
		pop_operand_stack(long,value);
		jvm_obj* obj = (jvm_obj*)value;
		/**
		 * record prev stack frame
		 */
		jvm_stack_frame* prev_frame = copy_stack_frame(cur_stack_frame);
		cur_stack_frame = method->method_code->current_frame;
		cur_stack_frame->return_address = vm_pc.pc+len;
		cur_stack_frame->prev = prev_frame;
		/**
		 * record pre interp env
		 *
		 */
		interpreter_env* prev_env = copy_interp_env(cur_interpreter_env);
		cur_interpreter_env->cp_info = method->klass->cp_info;
		cur_interpreter_env->prev = prev_env;

		vm_pc.pc = method->method_code->code;

		/**
		 * set args
		 */
		set_local_table(long,0,obj);
		if(arg_size > 0)
		{
			u2 idx;
			for(idx=1;idx<=arg_size;idx++)
			{
				set_local_table(long,idx,(*(values+idx-1)));
			}

			if(values != NULL){
				free(values);
			}
		}

		if(0 != interpreter_byte_code(method))
				exit(1);
		return 0;
	}

out:	vm_pc.pc += len;

	return 0;
}
int jvm_interp_invokespecial(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));

	/**
	 * get method
	 */
	class_method* method = get_class_method(index);
	if(!method)
	{
		//not found,skip
		vm_pc.pc += len;
		return 0;
	}

	/**
	 * get args
	 */
	char* desc = ((constant_utf8_info*)(method->klass->cp_info[method->descriptor_index].info))->bytes;
	int arg_size = get_arg_size(desc);

	long* value = get_invoke_args(arg_size);

	long v;
	pop_operand_stack(long,v);
	jvm_obj* obj = (jvm_obj*)v;
	if(!obj)
		exit(1);
	/**
	 * record prev stack frame
	 */
	jvm_stack_frame* prev_frame = copy_stack_frame(cur_stack_frame);
	cur_stack_frame = method->method_code->current_frame;
	cur_stack_frame->return_address = vm_pc.pc+len;
	cur_stack_frame->prev = prev_frame;

	/**
	 * record prev env
	 */
	interpreter_env* prev_env = copy_interp_env(cur_interpreter_env);
	cur_interpreter_env->cp_info = method->klass->cp_info;
	cur_interpreter_env->prev = prev_env;

	vm_pc.pc = method->method_code->code;

	/**
	 * set args
	 */
	set_local_table(long,0,obj);
	u2 idx;
	for(idx=1;idx<=arg_size;idx++)
	{
		set_local_table(long,idx,(*(value+idx-1)));
	}

	if(value != NULL){
		free(value);
	}

	if(0 != interpreter_byte_code(method))
		exit(1);

		//vm_pc.pc += len;
	return 0;
}

class_method* get_class_method(u2 method_index)
{

	u2 class_index = ((constant_method_info*)(cur_interpreter_env->cp_info[method_index].info))->class_index;
	u2 desc_index = ((constant_method_info*)(cur_interpreter_env->cp_info[method_index].info))->name_and_type_index;
	class_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_index].info))->bytes;
	if(0 == strcmp("java/lang/Object",class_name))
	{
		return NULL;
	}
	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->name_index;
	desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->descriptor_index;
	char* method_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;
	char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;

	class_method* method = find_class_method(&class_list_head,class_name,method_name,desc_name);
	if(!method)
	{
		CLASS* newClass = jvm_load_class(jvmargs->classpath,class_name);
		if(!newClass)
		{
			return NULL;
		}
		method = find_class_method(&class_list_head,class_name,method_name,desc_name);
	}

	return method;
}

int jvm_interp_invokestatic(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	u2 index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));

	/**
	 * get method
	 */
	class_method* method = get_class_method(index);
	if(!method)
	{
		exit(1);
	}

	/**
	 * get args
	 */
	char* desc = ((constant_utf8_info*)(cur_interpreter_env->cp_info[method->descriptor_index].info))->bytes;
	int arg_size = get_arg_size(desc);

	long* value = get_invoke_args(arg_size);

	/**
	 * record prev stack frame
	 */
	jvm_stack_frame* prev_frame = copy_stack_frame(cur_stack_frame);
	cur_stack_frame = method->method_code->current_frame;
	cur_stack_frame->return_address = vm_pc.pc+len;
	cur_stack_frame->prev = prev_frame;

	/**
	 * record prev env
	 */
	interpreter_env* prev_env = copy_interp_env(cur_interpreter_env);
	cur_interpreter_env->cp_info = method->klass->cp_info;
	cur_interpreter_env->prev = prev_env;

	vm_pc.pc = method->method_code->code;

	/**
	 * set args
	 */
	u2 idx;
	for(idx=0;idx<arg_size;idx++)
	{
		set_local_table(long,idx,(*(value+idx)));
	}

	if(NULL != value)
	{
		free(value);
	}
	if(0 != interpreter_byte_code(method))
		exit(1);


		//vm_pc.pc += len;
	return 0;
}
class_method* get_interface_method(u2 index)
{
	u2 class_index = ((constant_interface_info*)(cur_interpreter_env->cp_info[index].info))->class_index;
	u2 desc_index = ((constant_interface_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;
	class_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_index].info))->bytes;
	if(0 == strcmp("java/lang/Object",class_name))
	{
		return NULL;
	}
	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->name_index;
	desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[desc_index].info))->descriptor_index;
	char* method_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;
	char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;

	class_method* method = find_class_method(&class_list_head,class_name,method_name,desc_name);
	if(!method)
	{
		CLASS* newClass = jvm_load_class(jvmargs->classpath,class_name);
		if(!newClass)
		{
			return NULL;
		}
		method = find_class_method(&class_list_head,class_name,method_name,desc_name);
	}

	return method;
}

int jvm_interp_invokeinterface(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	u2 index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));
	/**
	 * get class method
	 */
	class_method* method = get_interface_method(index);
	if(!method)
	{
		//not found,skip
		vm_pc.pc += len;
		return 0;
	}
	/**
	 * get args
	 */
	char* desc = ((constant_utf8_info*)(method->klass->cp_info[method->descriptor_index].info))->bytes;
	int arg_size = get_arg_size(desc);

	long* value = get_invoke_args(arg_size);
	long v;
	pop_operand_stack(long,v);
	jvm_obj* obj = (jvm_obj*)v;
	if(!obj)
		exit(1);
	/**
	 * record prev stack frame
	 */
	jvm_stack_frame* prev_frame = copy_stack_frame(cur_stack_frame);

	int id;
	for(id=0;id<obj->class->methods_count;id++){
		class_method* real_method = obj->class->methods+id;
		if(0 == strcmp(real_method->method_name,method->method_name)){
			char* desc_name = ((constant_utf8_info*)(real_method->klass->cp_info[real_method->descriptor_index].info))->bytes;
			if(0 == strcmp(desc_name,desc)){
				method = real_method;
				break;
			}
		}
	}

	cur_stack_frame = method->method_code->current_frame;
	cur_stack_frame->return_address = vm_pc.pc+len;
	cur_stack_frame->prev = prev_frame;

	/**
	 * record prev env
	 */
	interpreter_env* prev_env = copy_interp_env(cur_interpreter_env);
	cur_interpreter_env->cp_info = method->klass->cp_info;
	cur_interpreter_env->prev = prev_env;

	vm_pc.pc = method->method_code->code;

	/**
	 * set args
	 */
	set_local_table(long,0,obj);
	u2 idx;
	for(idx=1;idx<=arg_size;idx++)
	{
		set_local_table(long,idx,(*(value+idx-1)));
	}

	if(value != NULL){
		free(value);
	}

	if(0 != interpreter_byte_code(method))
		exit(1);

		//vm_pc.pc += len;
	return 0;
}
int jvm_interp_invokedynamic(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}

jvm_stack_obj_list* add_object_to_cur_stack_list(jvm_stack_frame* cur_stack_frame,jvm_obj* obj)
{
	obj->ref_count++;
	jvm_stack_obj_list* o_list = (jvm_stack_obj_list*)malloc(sizeof(jvm_stack_obj_list));
	o_list->next = NULL;
	o_list->obj = obj;

	jvm_stack_obj_list* list = cur_stack_frame->list;
	if(NULL == list)
	{
		list = o_list;
	}else
	{
		jvm_stack_obj_list* p = list;
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = o_list;
	}

	return list;

}

int jvm_interp_new(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	CLASS* new_class;
	jvm_obj* object;
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 name_index = ((constant_class_info*)cur_interpreter_env->cp_info[index].info)->name_index;
	char* class_name = ((constant_utf8_info*)cur_interpreter_env->cp_info[name_index].info)->bytes;
	new_class = find_class(&class_list_head,class_name);
	if(new_class)
	{
		//already loaded
		goto object;
	}
	new_class = jvm_load_class(jvmargs->classpath,class_name);
	if(!new_class)
	{
		return -1;
	}

object:
	//object = (jvm_obj*)malloc(sizeof(jvm_obj));
	object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj));
	if(!object)
	{
		return -1;
	}
	object->isArray = -1;
	object->age = 0;
	object->class = new_class;
	object->ref_count = 0;
	object->type = -1;
	push_operand_stack(long,(long*)object);

	//
	cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);

		vm_pc.pc += len;
	return 0;
}
int jvm_interp_newarray(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	u2 index;
	index = (*(u1 *)(info + 1));
	int count;
	pop_operand_stack(int,count);
	u2 idx;

	//jvm_obj* obj = (jvm_obj*)malloc(sizeof(jvm_obj));
	jvm_obj* obj = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj));
	obj->ref_count = 0;
	cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,obj);
	obj->isArray = 1;

	CLASS* cl;
	jvm_obj* object;

	switch(index){
	case T_BOOLEAN:
	case T_BYTE:
	case T_INT:
	case T_SHORT:
		//object = (jvm_obj*)malloc(sizeof(jvm_obj)*count);
		object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
		object->ref_count = 0;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
		for(idx=0;idx<count;idx++){
			(object+idx)->type = T_INT;
			(object+idx)->value = 0;
			(object+idx)->isArray = 0;
		}
		obj->type=T_INT;
		cl = find_class(&class_list_head,"[I");
		if(!cl){
			cl = jvm_load_class(jvmargs->classpath,"[I");
		}
		obj->class = cl;
		obj->value=object;
		push_operand_stack(long,(long*)obj);
		break;
	case T_CHAR:
		//object = (jvm_obj*)malloc(sizeof(jvm_obj)*count);
		object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
		object->ref_count = 0;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
		for(idx=0;idx<count;idx++){
			(object+idx)->type = T_CHAR;
			(object+idx)->value = '\0';
			(object+idx)->isArray = 0;
		}
		obj->type = T_CHAR;
		cl = find_class(&class_list_head,"[C");
		if(!cl){
			cl = jvm_load_class(jvmargs->classpath,"[C");
		}
		obj->class = cl;
		obj->value=object;
		push_operand_stack(long,(long*)obj);
		break;
	case T_FLOAT:
		//object = (jvm_obj*)malloc(sizeof(jvm_obj)*count);
		object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
		object->ref_count = 0;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
		for(idx=0;idx<count;idx++){
			(object+idx)->type = T_FLOAT;
			(object+idx)->value = 0.0f;
			(object+idx)->isArray = 0;
		}
		obj->type = T_FLOAT;
		cl = find_class(&class_list_head,"[F");
		if(!cl){
			cl = jvm_load_class(jvmargs->classpath,"[F");
		}
		obj->class = cl;
		obj->value=object;
		push_operand_stack(long,(long*)obj);
		break;
	case T_LONG:
		//object = (jvm_obj*)malloc(sizeof(jvm_obj)*count);
		object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
		object->ref_count = 0;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
		for(idx=0;idx<count;idx++){
			(object+idx)->type = T_LONG;
			(object+idx)->value = 0l;
			(object+idx)->isArray = 0;
		}
		cl = find_class(&class_list_head,"[L");
		if(!cl){
			cl = jvm_load_class(jvmargs->classpath,"[L");
		}
		obj->class = cl;
		obj->value=object;
		push_operand_stack(long,(long*)obj);
		break;
	case T_DOUBLE:
		//object = (jvm_obj*)malloc(sizeof(jvm_obj)*count);
		object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
		object->ref_count = 0;
		cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
		for(idx=0;idx<count;idx++){
			(object+idx)->type = T_DOUBLE;
			(object+idx)->value = 0.0;
			(object+idx)->isArray = 0;
		}
		obj->type = T_DOUBLE;
		cl = find_class(&class_list_head,"[D");
		if(!cl){
			cl = jvm_load_class(jvmargs->classpath,"[D");
		}
		obj->class = cl;
		obj->value=object;
		push_operand_stack(long,(long*)obj);
		break;
	default:
		break;
	}

		vm_pc.pc += len;
	return 0;
}
int jvm_interp_anewarray(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	int count;
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 name_index = ((constant_class_info*)cur_interpreter_env->cp_info[index].info)->name_index;
	char* class_name = ((constant_utf8_info*)cur_interpreter_env->cp_info[name_index].info)->bytes;
	CLASS* cl = find_class(&class_list_head,class_name);
	if(cl)
	{
		//already loaded
		goto out;
	}
	cl = jvm_load_class(jvmargs->classpath,class_name);
	jvm_obj* object;
	u2 idx;

out:
	pop_operand_stack(int,count);
	//object = (jvm_obj*)malloc(sizeof(jvm_obj));
	object = (jvm_obj*)mem_alloc(jvm_mem_addr,sizeof(jvm_obj));
	object->ref_count = 0;
	cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,object);
	jvm_obj* p = object;
	p->age = 0;
	p->class = cl;
	p->isArray = 1;
	//void* value = malloc(sizeof(jvm_obj)*count);
	jvm_obj* value = mem_alloc(jvm_mem_addr,sizeof(jvm_obj)*count);
	value->ref_count = 0;
	value->age = 0;
	value->isArray = 0;
	cur_stack_frame->list = add_object_to_cur_stack_list(cur_stack_frame,value);
	p->value = value;
	push_operand_stack(long,object);

		vm_pc.pc += len;
	return 0;
}
int jvm_interp_arraylength(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_athrow(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_checkcast(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}

int interface_instanceof(CLASS* c1,CLASS* c2)
{
	CLASS* temp_class = c1;
	u2 idx;
	for(idx=0; idx<temp_class->interfaces_count;idx++){
		u2 index = temp_class->interfaces[idx];
		u2 name_index = ((constant_class_info*)(temp_class->cp_info[index].info))->name_index;
		char * class_name = ((constant_utf8_info*)(temp_class->cp_info[name_index].info))->bytes;
		CLASS* c = find_class(&class_list_head,class_name);
		if(!c)
		{
			c = jvm_load_class(jvmargs->classpath,class_name);
		}
		if(c == c2)
		{
			return 1;
		}else
		{
			if(NULL == c)
				return 0;
			return interface_instanceof(c,c2);
		}

	}
}

int jvm_interp_instanceof(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 name_index = ((constant_class_info*)cur_interpreter_env->cp_info[index].info)->name_index;
	char* class_name = ((constant_utf8_info*)cur_interpreter_env->cp_info[name_index].info)->bytes;

	CLASS* cl = find_class(&class_list_head,class_name);
	if(!cl){
		cl = jvm_load_class(jvmargs->classpath,class_name);
	}

	long tmp;
	pop_operand_stack(long,tmp);
	jvm_obj* obj = (jvm_obj*)tmp;
	CLASS* obj_class = obj->class;
	int result = 0;
	if(cl == obj_class)
	{
		result = 1;
	}else{//todo need detail test
		if(1 != obj->isArray)
		{
			if(obj_class->access_flags & ACC_INTERFACE)//interface TODO when this happens?s
			{
				/**
				 *
				If S is an interface type, then:

					If T is a class type, then T must be Object.

					If T is an interface type, then T must be the same interface as S or a superinterface of S.

				 *
				 */
				if(cl->access_flags & ACC_INTERFACE)
				{
					result = interface_instanceof(obj_class,cl);
				}

			}else{
				/**
				 *
				If S is an ordinary (nonarray) class, then:
					If T is a class type, then S must be the same class as T, or S must be a subclass of T;
					If T is an interface type, then S must implement interface T.
				*
				*/
				if(cl->access_flags & ACC_INTERFACE)
				{
					CLASS* s = obj_class;
					while(s){
						result = interface_instanceof(s,cl);
						if(result)
							break;
						s = s->super;
					}

				}else
				{
					CLASS* super = obj_class->super;
					while(super)
					{
						if(super == cl)
						{
							result = 1;
							break;
						}
						super = super->super;
					}
				}

			}

		}else{//array
			/**
			 *
			If S is a class representing the array type SC[], that is, an array of components of type SC, then:

				If T is a class type, then T must be Object.

				If T is an interface type, then T must be one of the interfaces implemented by arrays (JLS §4.10.3).

				If T is an array type TC[], that is, an array of components of type TC, then one of the following must be true:

					TC and SC are the same primitive type.

					TC and SC are reference types, and type SC can be cast to TC by these run-time rules.
			 *
			 */

			if(cl->isArray = 1){
				//If T is an array type TC[], that is, an array of components of type TC, then one of the following must be true:
				if((obj_class->primitive_type == cl->primitive_type) && obj_class->primitive_type!=-1){
					if(obj_class->dimensions == cl->dimensions)
						result = 1;
				}else{
					if((obj_class->dimensions == cl->dimensions) || (obj_class->dimensions == 0 && cl->dimensions==1)){
						//cast

						char* p = obj_class->class_name;
						if(*p == '['){
							while(*p=='['){
								p++;
							}
							if(*p == 'L'){
								p++;
							}
							int len = strlen(p);
							if(*(p+len-1)==';'){
								*(p+len-1) = '\0';
							}

							obj_class = find_class(&class_list_head,p);
							if(!obj_class){
								obj_class = jvm_load_class(jvmargs->classpath,p);
							}
						}

						p = cl->class_name;
						if(*p == '['){
							while(*p=='['){
								p++;
							}
							if(*p == 'L'){
								p++;
							}
							int len = strlen(p);
							if(*(p+len-1)==';'){
								*(p+len-1) = '\0';
							}

							cl = find_class(&class_list_head,p);
							if(!cl){
								cl = jvm_load_class(jvmargs->classpath,p);
							}
						}

						if(obj_class == cl){
							result = 1;
						}else{
							CLASS* c1 = obj_class->super;
							while(c1){
								if(c1 == cl){
									result = 1;
									break;
								}
								c1 = c1->super;
							}
							if(!result){
								CLASS* s = obj_class;
								while(s){
									result = interface_instanceof(s,cl);
									if(result)
										break;
									s = s->super;
								}
							}
						}
					}
				}
			}


		}

	}
	push_operand_stack(int,result);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_monitorenter(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_monitorexit(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_wide(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}

jvm_obj* malloc_array(int i,int n,int a[])
{
	if(i>=n)
		return 0;
	jvm_obj* jArray = (jvm_obj*)malloc(sizeof(jvm_obj)*a[i]);
	int j;
	for(j=0;j<a[i];j++)
	{
		jArray[j].value = malloc_array(i+1,n,a);
	}

	return jArray;
}

int jvm_interp_multianewarray(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index;
	index = (*(u1 *)(info + 1) << 8) | (*(u1 *)(info + 2));
	u2 dimensions;
	dimensions = *((u1*)(info+3));
	int* count = (int*)malloc(sizeof(int)*dimensions);
	count += dimensions-1;
	int d = dimensions;
	while(d--){
		pop_operand_stack(int,(*count));
		count--;
	}
	int* a = count+1;

	u2 name_index = ((constant_class_info*)cur_interpreter_env->cp_info[index].info)->name_index;
	char* class_name = ((constant_utf8_info*)cur_interpreter_env->cp_info[name_index].info)->bytes;
	CLASS* cl = find_class(&class_list_head,class_name);
	if(!cl){
		cl = jvm_load_class(jvmargs->classpath,class_name);
	}

	jvm_obj* obj = (jvm_obj*)malloc(sizeof(jvm_obj));
	jvm_obj* object = malloc_array(0,dimensions,a);
	switch(cl->primitive_type)
	{
	case T_INT:
	case T_BYTE:
	case T_SHORT:
	case T_BOOLEAN:
		obj->type = T_INT;
		break;
	case T_CHAR:
		obj->type = T_CHAR;
		break;
	case T_FLOAT:
		obj->type= T_FLOAT;
		break;
	case T_LONG:
		obj->type = T_LONG;
		break;
	case T_DOUBLE:
		obj->type = T_DOUBLE;
		break;
	}

	obj->age = 0;
	obj->class = cl;
	obj->isArray = 1;
	obj->ref_count = 0;
	obj->value = object;

	push_operand_stack(long,obj);

	vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifnull(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_ifnonnull(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_goto_w(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_jsr_w(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
		vm_pc.pc += len;
	return 0;
}
int jvm_interp_getfield(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);

	u2 index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));

	u2 name_and_type = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;

	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[name_and_type].info))->name_index;
	char* field_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;

	long addr;
	pop_operand_stack(long,addr);
	jvm_obj* obj = (jvm_obj*)addr;
	obj_field_value * fields = obj->field_values;
	while(0 != strcmp(fields->field_name,field_name))
	{
		fields = fields->next;
	}
	long value = fields->value;
	push_operand_stack(long,value);

		vm_pc.pc += len;
	return 0;
}
int jvm_interp_putfield(u2 len, char *code_name, void *info)
{
	print_interp_info(code_name);
	u2 index = ((*(u1 *)(info + 1)) << 8) | (*(u1 *)(info + 2));
	u2 class_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->class_index;
	u2 name_type_index = ((constant_field_info*)(cur_interpreter_env->cp_info[index].info))->name_and_type_index;
	class_index = ((constant_class_info*)(cur_interpreter_env->cp_info[class_index].info))->name_index;
	char* class_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[class_index].info))->bytes;

	u2 name_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[name_type_index].info))->name_index;
	u2 desc_index = ((constant_name_type_info*)(cur_interpreter_env->cp_info[name_type_index].info))->descriptor_index;
	char* field_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[name_index].info))->bytes;
	char* desc_name = ((constant_utf8_info*)(cur_interpreter_env->cp_info[desc_index].info))->bytes;

	int is_primitive_type = is_primitive_desc_type(desc_name);

	class_field* field = find_class_field(&class_list_head,class_name,field_name);

	long value;
	long addr;
	pop_operand_stack(long,value);
	pop_operand_stack(long,addr);

	jvm_obj* obj = (jvm_obj*)addr;
	if(NULL == obj->field_values)
	{
		//not exists
		obj->field_values = (obj_field_value*)malloc(sizeof(obj_field_value));//less than 32,malloc directly
		if(!obj->field_values)
			exit(1);
		obj_field_value* v = obj->field_values;
		v->field_name = field->field_name;
		v->field_desc = desc_name;
		v->next = NULL;
		v->value = value;

		if(!is_primitive_type)
		{
			jvm_obj* o = (jvm_obj*)value;
			o->ref_count++;
		}
	}
	else
	{
		obj_field_value* values = obj->field_values;
		obj_field_value* prev = values;
		int flag = 0;
		while(NULL != values)
		{
			if(0 == strcmp(values->field_name,field->field_name))
			{
				long ve = values->value;
				if(!is_primitive_type)
				{
					jvm_obj* o = (jvm_obj*)ve;
					o->ref_count--;
				}
				values->value = value;
				flag = 1;
				break;
			}
			prev = values;
			values = values->next;
		}
		if(!flag)
		{
			obj_field_value* vv = (obj_field_value*)malloc(sizeof(obj_field_value));
			vv->field_name = field->field_name;
			vv->field_desc = desc_name;
			vv->next = NULL;
			vv->value = value;
			prev->next = vv;
			if(!is_primitive_type)
			{
				jvm_obj* o = (jvm_obj*)vv;
				o->ref_count++;
			}
		}
	}




		vm_pc.pc += len;
	return 0;
}




int print_method_code(class_method* method)
{
	u4 idx;
	u1* code = method->method_code->code;
	char buf[256];
	memset(buf,'\0',256);
	sprintf(buf,"=============method:%s code====================\n",method->method_name);
	write_log(buf);
	for(idx=0;idx<method->method_code->code_length;idx++)
	{
		memset(buf,'\0',256);
		u1 c = (*code);
		char* code_name = jvm_byte_code[c].code_name;
		char * p = buf;
		sprintf(p,"\n%s ",code_name);
		p+=strlen(p);

		u2 c_len = jvm_byte_code[c].code_len;
		u2 i;
		u2 r = 0;
		for(i=1;i<c_len;i++)
		{
			code++;
			r = (r<<8)+(*code);
		}
		if(0 != r)
		{
			sprintf(p,"0x%x",r);

		}
		write_log(buf);
		idx += (c_len-1);
		code++;
	}
	write_log("\n");
	memset(buf,'\0',256);
	sprintf(buf,"=============method:%s code end====================\n",method->method_name);
	write_log(buf);

	return 0;

}

int interpreter_byte_code(class_method* method)
{
	print_method_code(method);

	cur_stack_frame = method->method_code->current_frame;
	cur_interpreter_env->cp_info = method->klass->cp_info;
	//cur_interpreter_env->prev = NULL;

	jvm_stack_depth++;

	while(1)
	{
		if(0 == jvm_stack_depth)
		{
			break;
		}

		u1 index = *(vm_pc.pc);
		jvm_byte_code[index].func(jvm_byte_code[index].code_len,jvm_byte_code[index].code_name,vm_pc.pc);
	}

	return 0;
}
