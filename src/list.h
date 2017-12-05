/*
 * list.h
 *
 *
 *      Author: chengzhang
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct LIST{
	struct LIST* next;
	void* data;
	struct LIST* prev;
}list;

////////////////////////////////list/////////////////////////////////////////
list class_list_head;//存储class信息，后面如果加载jdk类库后，查找class信息性能是个问题，需要修改下

#endif /* LIST_H_ */
