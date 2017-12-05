/*
 * classloader.h
 *
 *
 *      Author: chengzhang
 */

#ifndef CLASSLOADER_H_
#define CLASSLOADER_H_

#include "type.h"
#include "czvm.h"
#include "list.h"

/**
 * 一次读取class文件中的4个字节
 */
#define CLASS_READ_U4(c,p)  \
	do{		\
		c = (((*(u4*)p)&0x000000ff)<<24) | (((*(u4*)p)&0x0000ff00)<<8) \
			| (((*(u4*)p)&0xff000000)>>24) | (((*(u4*)p)&0x00ff0000)>>8); \
		p +=4;	\
	}while (0);

/**
 * 一次读取class文件中的2个字节
 */
#define CLASS_READ_U2(c,p) \
	do{	\
		c = (((*(u2*)p)&0x00ff)<<8) | (((*(u2*)p)&0xff00)>>8); \
		p += 2; \
	}while(0);

/**
 * 一次读取class文件中的1个字节
 */
#define CLASS_READ_U1(c,p) \
	do{	\
		c = (*(u1*)p); \
		p += 1;	\
	}while(0);

#endif /* CLASSLOADER_H_ */
