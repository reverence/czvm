/*
 * czvm.c
 *
 *
 *      Author: chengzhang
 */

#include "czvm.h"
#include "list.h"
#include <string.h>
#include "interpreter.h"
#include <getopt.h>
#include <unistd.h>
#include "log.h"
#include "mem.h"


int jvm_args_init(char **argv)
{
	jvmargs = (jvmArgs*)malloc(sizeof(jvmArgs));
	if(!jvmargs)
	{
		printf("malloc jvmArgs failed");
		return -1;
	}
	memset(jvmargs->classpath, '\0', 1024);
	strcpy(jvmargs->classpath, argv[2]);
	memset(jvmargs->log_path,'\0',1024);
	strcpy(jvmargs->log_path, JVM_LOG_PATH);

	if(0 == strcmp(".",argv[2]))
	{
		getcwd(jvmargs->classpath,sizeof(jvmargs->classpath));
	}
	return 0;
}

int jvm_init(jvmArgs *jvmargs,char *classname)
{

	cur_interpreter_env = (interpreter_env*)malloc(sizeof(interpreter_env));
	if(!cur_interpreter_env)
	{
		return -1;
	}

	char *classpath = jvmargs->classpath;
	CLASS* cl = jvm_load_class(classpath,classname);
	if(!cl){
		return -1;
	}

	return 0;

}

int jvm_run(char* classname)
{
	//find class info by classname
	CLASS* klass = NULL;
	class_method* method;

	method = find_class_method(&class_list_head,classname,"main","");
	if(!method)
	{
		return -1;
	}

	vm_pc.pc = method->method_code->code;

	if(-1 == interpreter_byte_code(method))
	{
		return -1;
	}

	return 0;
}

void jvm_usage(const char *proc)
{
	printf("usage: %s <option> [class_path] [program]\n","czvm");
	printf("option:\n");
	printf("-cp [class_path]\t\texec java bytecode.\n");
	printf("-v\t\t\t show jvm version.\n");
}

void jvm_banner()
{
	printf("czvm v%2.2f\t%s\n", JVM_VERSION, JVM_BANNER);
}

int jvm_mem_init()
{
	jvm_mem_addr = init_jvm_mem();
	if(!jvm_mem_addr)
	{
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		jvm_usage(argv[0]);
		return 0;
	}

	if(argc == 2)
	{
		if(0 == strcmp("-v",argv[1]))
		{
			jvm_banner();
			return 0;
		}else
		{
			return -1;
		}
	}

	//jvm args init
	if(-1 == jvm_args_init(argv))
	{
		return -1;
	}

	if(-1 == log_init(jvmargs->log_path))
	{
		return -1;
	}

	if(-1 == jvm_mem_init())
	{
		return -1;
	}

	//jvm init
	if(-1 == jvm_init(jvmargs,argv[3]))
	{
		log_destory();
		printf("jvm init failed");
		return -1;
	}

	//jvm run

	if(-1 == jvm_run(argv[3]))
	{
		log_destory();
		printf("jvm run failed");
		return -1;
	}
	log_destory();
	return 0;
}
