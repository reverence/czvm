/*
 * log.h
 *
 *
 *      Author: chengzhang
 */

#ifndef LOG_H_
#define LOG_H_


///////////////////////////////function///////////////////////////////

int log_init(char* log_path);

void log_destory();

void write_log(char* content);

#endif /* LOG_H_ */
