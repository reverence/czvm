/*
 * type.c
 *
 *
 *      Author: chengzhang
 */
#include "type.h"

int is_primitive_desc_type(char* desc_name)
{
	return !(strcmp("I",desc_name) && strcmp("C",desc_name) && strcmp("D",desc_name) && strcmp("Z",desc_name) && strcmp("F",desc_name) && strcmp("J",desc_name));
}
