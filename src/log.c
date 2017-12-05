/*
 * log.c
 *
 *
 *      Author: chengzhang
 */


#include <stdio.h>
#include <time.h>
#include "log.h"
#include <sys/stat.h>

FILE* log_fp;


int log_init(char* log_path)
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p =localtime(&timep);

	char log[1024];
	memset(log,'\0',1024);

	snprintf(log,sizeof(log),"%s/%d-%02d-%02d.log",log_path,1900+p->tm_year,1+p->tm_mon,p->tm_mday);

	log_fp = fopen(log, "w+");
	if(!log_fp)
	{
		free(p);
		free(log);
		return -1;
	}

	return 0;
}

void write_log(char* content)
{
	fflush(log_fp);
	fprintf(log_fp,"%s\n",content);
	//printf(content);
}

void log_destory()
{
	fclose(log_fp);
}
