/*
 * mem.h
 *
 *
 *      Author: chengzhang
 */

#ifndef MEM_H_
#define MEM_H_

#include<stddef.h>

#define CHUNK_ARRAY_SIZE 16

#define SLOT_ARRAY_SIZE_PER_CHUNK 32

#define CHUNK_GC_FACTOR 0.6


typedef struct CHUNK_SLOT
{
    struct CHUNK_SLOT         *next               ;
    unsigned char             *star               ;
    unsigned char             *end                ;
    int               slot_length        ;
    int chunk_index;
    int slot_index_in_chunk;//used to split or merge slots
}chunk_slot;

typedef struct MEM_CHUNK
{
	int       per_slot_size;
	chunk_slot       *empty_slots;
	chunk_slot       *used_slots;
	int       slots_array_num;
	long used_size;
	long empty_size;

}mem_chunk;

typedef struct MEM_SLAB
{
	mem_chunk *chunk_array;
	long total_allocted;
	int chunk_array_size;
	long total_used;
}jvm_mem;

/****************************************global variable*********************************************************/
jvm_mem* jvm_mem_addr;

/********************************function***********************************************************/
jvm_mem* init_jvm_mem();

void* mem_alloc(jvm_mem* mem , size_t size);

#endif /* MEM_H_ */
