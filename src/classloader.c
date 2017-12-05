/*
 * classloader.c
 *
 *
 *      Author: chengzhang
 */


#include "classloader.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <memory.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

static char* mem_start;
static char* class_start_mem;
static int class_fd;

CLASS* find_class(list* head,char* classname);

int parse_class_magic(CLASS** klass)
{
	CLASS_READ_U4((*klass)->magic,mem_start)
	if(JVM_CLASS_MAGIC != (*klass)->magic)
	{
		printf("magic not correct");
		return -1;
	}
	return 0;
}

int parse_class_version(CLASS** klass)
{
	CLASS_READ_U2((*klass)->minor_version,mem_start)
	CLASS_READ_U2((*klass)->major_version,mem_start)

	//check?

	return 0;
}

int allocate_constant_pool(CLASS** klass)
{
	constant_pool_info* cp_info = (constant_pool_info*)malloc(sizeof(constant_pool_info) * (*klass)->constant_pool_count);
	if(!cp_info)
	{
		return -1;
	}
	(*klass)->cp_info = cp_info;
	return 0;
}

int parse_constant_method(u1 tag,u2* index,CLASS** klass)
{
	constant_method_info* method = (constant_method_info*)malloc(sizeof(constant_method_info));
	if(!method)
	{
		printf("malloc method info failed");
		return -1;
	}
	CLASS_READ_U2(method->class_index,mem_start)
	CLASS_READ_U2(method->name_and_type_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)method;

	return 0;
}

int parse_constant_class(u1 tag,u2* index,CLASS** klass)
{
	constant_class_info* class_inf = (constant_class_info*)malloc(sizeof(constant_class_info));
	if(!class_inf)
	{
		return -1;
	}

	CLASS_READ_U2(class_inf->name_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)class_inf;

	return 0;
}

int parse_constant_utf8(u1 tag,u2* index,CLASS** klass)
{
	constant_utf8_info* utf8 = (constant_utf8_info*)malloc(sizeof(constant_utf8_info));
	if(!utf8)
	{
		return -1;
	}
	CLASS_READ_U2(utf8->length,mem_start)
	u1* buf = (u1*)malloc(sizeof(u1)*(utf8->length+1));
	if(!buf)
	{
		free(utf8);
		return -1;
	}
	memset(buf,'\0',utf8->length+1);
	memcpy(buf,mem_start,utf8->length);
	mem_start += utf8->length;
	utf8->bytes = buf;

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)utf8;

	return 0;
}

int parse_constant_name_type(u1 tag,u2* index,CLASS** klass)
{
	constant_name_type_info* name_type = (constant_name_type_info*)malloc(sizeof(constant_name_type_info));
	if(!name_type)
	{
		return -1;
	}

	CLASS_READ_U2(name_type->name_index,mem_start)
	CLASS_READ_U2(name_type->descriptor_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)name_type;

	return 0;
}

int parse_constant_string(u1 tag,u2* index,CLASS** klass)
{
	constant_string_info* str = (constant_string_info*)malloc(sizeof(constant_string_info));
	if(!str)
	{
		return -1;
	}

	CLASS_READ_U2(str->string_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)str;

	return 0;
}

int parse_constant_integer(u1 tag,u2* index,CLASS** klass)
{
	constant_integer_info* integer = (constant_integer_info*)malloc(sizeof(constant_integer_info));
	if(!integer)
	{
		return -1;
	}

	CLASS_READ_U4(integer->bytes,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)integer;
}

int parse_constant_float(u1 tag,u2* index,CLASS** klass)
{
	constant_float_info* f = (constant_integer_info*)malloc(sizeof(constant_integer_info));
	if(!f)
	{
		return -1;
	}

	CLASS_READ_U4(f->bytes,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)f;

	return 0;
}

int parse_constant_long(u1 tag,u2* index,CLASS** klass)
{
	constant_long_info* l = (constant_long_info*)malloc(sizeof(constant_long_info));
	if(!l)
	{
		return -1;
	}

	CLASS_READ_U4(l->high_bytes,mem_start)
	CLASS_READ_U4(l->low_bytes,mem_start)

	(*klass)->cp_info[(*index)].index = index;
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)l;

	return 0;
}

int parse_constant_double(u1 tag,u2* index,CLASS** klass)
{
	constant_double_info* d = (constant_long_info*)malloc(sizeof(constant_double_info));
	if(!d)
	{
		return -1;
	}

	CLASS_READ_U4(d->high_bytes,mem_start)
	CLASS_READ_U4(d->low_bytes,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)d;

	return 0;
}

int parse_constant_method_handle(u1 tag,u2* index,CLASS** klass)
{
	constant_methodHandle_info* method_handle = (constant_methodHandle_info*)malloc(sizeof(constant_methodHandle_info));
	if(!method_handle)
	{
		return -1;
	}

	CLASS_READ_U1(method_handle->reference_kind,mem_start)
	CLASS_READ_U2(method_handle->reference_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)method_handle;

	return 0;
}

int parse_constant_method_type(u1 tag,u2* index,CLASS** klass)
{
	constant_methodtype_info* method_type = (constant_methodtype_info*)malloc(sizeof(constant_methodtype_info));
	if(!method_type)
	{
		return -1;
	}

	CLASS_READ_U2(method_type->descriptor_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)method_type;

	return 0;
}

int parse_constant_invoke_dynamic(u1 tag,u2* index,CLASS** klass)
{
	constant_invoke_dynamic_info* invoke_dynamic = (constant_invoke_dynamic_info*)malloc(sizeof(constant_invoke_dynamic_info));
	if(!invoke_dynamic)
	{
		return -1;
	}

	CLASS_READ_U2(invoke_dynamic->bootstrap_method_attr_index,mem_start)
	CLASS_READ_U2(invoke_dynamic->name_and_type_index,mem_start)

	(*klass)->cp_info[(*index)].index = (*index);
	(*klass)->cp_info[(*index)].tag = tag;
	(*klass)->cp_info[(*index)].info = (u1*)invoke_dynamic;
	return 0;
}

int parse_constant_attribute(CLASS** klass ,u2* index)
{
	u1 constant_tag;
	CLASS_READ_U1(constant_tag,mem_start)

	switch(constant_tag)
	{
	case CONSTANT_Fieldref:
	case CONSTANT_InterfaceMethodref:
	case CONSTANT_Methodref:
		if(-1 == parse_constant_method(constant_tag,index,klass))
		{
			return -1;
		}
		break;

	case CONSTANT_Class:
		if(-1 == parse_constant_class(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_Utf8:
		if(-1 == parse_constant_utf8(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_NameAndType:
		if(-1 == parse_constant_name_type(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_String:
		if(-1 == parse_constant_string(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_Integer:
		if(-1 == parse_constant_integer(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_Long:
		if(-1 == parse_constant_long(constant_tag,index,klass))
		{
			return -1;
		}
		(*index)++;
		break;
	case CONSTANT_Float:
		if(-1 == parse_constant_float(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_Double:
		if(-1 == parse_constant_double(constant_tag,index,klass))
		{
			return -1;
		}
		(*index)++;
		break;
	case CONSTANT_MethodHandle:
		if(-1 == parse_constant_method_handle(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_MethodType:
		if(-1 == parse_constant_method_type(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	case CONSTANT_InvokeDynamic:
		if(-1 == parse_constant_invoke_dynamic(constant_tag,index,klass))
		{
			return -1;
		}
		break;
	default:
		return -1;
	}

	return 0;
}

int parse_constant_pool(CLASS** klass)
{
	CLASS_READ_U2((*klass)->constant_pool_count,mem_start);
	if(-1 == allocate_constant_pool(klass)){
		printf("allocate memory for constant pool failed");
		return -1;
	}

	u2 index;

	for(index=1;index < (*klass)->constant_pool_count;index++)
	{
		if(-1 == parse_constant_attribute(klass,&index))
		{
			printf("parse constant attribute failed");
			return -1;
		}
	}

	return 0;
}

int parse_class_interfaces(CLASS** klass)
{
	CLASS_READ_U2((*klass)->interfaces_count,mem_start)
	if((*klass)->interfaces_count > 0)
	{
		u2* inf = (u2*)malloc(sizeof(u2)*(*klass)->interfaces_count);
		if(!inf)
		{
			return -1;
		}
		u2 index;
		for(index=0;index < ((*klass)->interfaces_count);index++)
		{
			CLASS_READ_U2(inf[index],mem_start)
		}
		(*klass)->interfaces=inf;
	}
	return 0;
}

int _parse_constant_value(class_field* field,u2 name_index)
{
	field_constantValue* cv = (field_constantValue*)malloc(sizeof(field_constantValue));
	if(!cv)
	{
		return -1;
	}
	cv->attribute_name_index = name_index;
	CLASS_READ_U4(cv->attribute_length,mem_start)
	CLASS_READ_U2(cv->constantvalue_index,mem_start)

	field->constantValue_attribute = cv;
}

int _parse_synthetic(class_field* field,u2 name_index)
{
	synthetic* s = (synthetic*)malloc(sizeof(synthetic));
	if(!s)
	{
		return -1;
	}
	s->attribute_name_index = name_index;
	CLASS_READ_U4(s->attribute_length,mem_start)

	field->synthetic_attribute = s;
}

int _parse_signature(class_field* field, u2 name_index)
{
	signature* s = (signature*)malloc(sizeof(signature));
	if(!s)
	{
		return -1;
	}
	s->attribute_name_index = name_index;
	CLASS_READ_U4(s->attribute_length,mem_start)
	CLASS_READ_U2(s->signature_index,mem_start)

	field->signature_attribute = s;
}

int _parse_deprecated(class_field* field, u2 name_index)
{
	deprecated* d = (deprecated*)malloc(sizeof(deprecated));
	if(!d)
	{
		return -1;
	}
	d->attribute_name_index = name_index;
	CLASS_READ_U4(d->attribute_length,mem_start)

	field->deprecated_attribute = d;
}

int parse_class_fields(CLASS** klass)
{
	CLASS_READ_U2((*klass)->fields_count,mem_start)
	if((*klass)->fields_count > 0)
	{
		class_field* fields = (class_field*)malloc(sizeof(class_field) * (*klass)->fields_count);
		if(!fields)
		{
			return -1;
		}
		u2 index;
		for(index=0;index < ((*klass)->fields_count);index++)
		{
			CLASS_READ_U2(fields[index].access_flags,mem_start)
			CLASS_READ_U2(fields[index].name_index,mem_start)
			CLASS_READ_U2(fields[index].descriptor_index,mem_start)
			CLASS_READ_U2(fields[index].attributes_count,mem_start)

			fields[index].field_name = ((constant_utf8_info*)((*klass)->cp_info[fields[index].name_index].info))->bytes;

			if(fields[index].attributes_count>0)
			{
				u2 idx;
				for(idx=0;idx<fields[index].attributes_count;idx++)
				{
					u2 name_index;
					CLASS_READ_U2(name_index,mem_start)
					//must be a CONSTANT_Utf8_info
					char* name = ((constant_utf8_info*)((*klass)->cp_info[name_index].info))->bytes;
					if(strcmp(name,"ConstantValue") == 0)
					{
						if(-1 == _parse_constant_value(&(fields[index]),name_index))
						{
							return -1;
						}
					}else if(strcmp(name,"Synthetic") == 0)
					{
						if(-1 == _parse_synthetic(&(fields[index]),name_index))
						{
							return -1;
						}

					}else if(strcmp(name,"Signature") == 0)
					{
						if(-1 == _parse_signature(&(fields[index]),name_index))
						{
							return -1;
						}

					}else if(strcmp(name,"Deprecated") == 0)
					{
						if(-1 == _parse_deprecated(&(fields[index]),name_index))
						{
							return -1;
						}

					}else if(strcmp(name,"RuntimeVisibleAnnotations") == 0)
					{
						//not so simple,deal later
					}else if(strcmp(name,"RuntimeInvisibleAnnotations")==0)
					{
						//not so simple,deal later
					}
				}
			}
		}

		(*klass)->fields = fields;
	}

	return 0;
}

int _parse_class_method_ex_table(exception_table* ex_table)
{
	CLASS_READ_U2(ex_table->start_pc,mem_start)
	CLASS_READ_U2(ex_table->end_pc,mem_start)
	CLASS_READ_U2(ex_table->handler_pc,mem_start)
	CLASS_READ_U2(ex_table->catch_type,mem_start)
}

int _pase_class_method_code_line_nubmer_table(method_code* code,u2 name_index)
{
	LineNumberTable* line = (LineNumberTable*)malloc(sizeof(LineNumberTable));
	if(!line)
	{
		return -1;
	}
	line->attribute_name_index = name_index;
	CLASS_READ_U4(line->attribute_length,mem_start)
	CLASS_READ_U2(line->line_number_table_length,mem_start)
	if(line->line_number_table_length > 0)
	{
		line_number_table* table = (line_number_table*)malloc(sizeof(line_number_table) * line->line_number_table_length);
		if(!table)
		{
			free(line);
			return -1;
		}
		u4 index;
		for(index=0;index<line->line_number_table_length;index++)
		{
			line_number_table* t = (&(table[index]));
			CLASS_READ_U2(t->start_pc,mem_start)
			CLASS_READ_U2(t->line_number,mem_start)
		}
		line->line_number_table = table;
	}
	code->lineNumberTable = line;

	return 0;
}

int init_method_stack_frame(method_code* code)
{
	jvm_stack_frame* stack_frame = (jvm_stack_frame*)malloc(sizeof(jvm_stack_frame));
	if(!stack_frame)
	{
		return -1;
	}
	u1* local_table = (u1*)malloc(sizeof(u1)* STACK_ITEM_SIZE * code->max_locals);
	if(!local_table)
	{
		free(stack_frame);
		return -1;
	}

	u1* op_stack = (u1*)malloc(sizeof(u1)* STACK_ITEM_SIZE * code->max_stack);
	if(!op_stack)
	{
		free(local_table);
		free(stack_frame);
		return -1;
	}

	stack_frame->local_variables = local_table;
	stack_frame->operand_stacks = op_stack;
	stack_frame->prev = NULL;
	stack_frame->return_address = NULL;
	stack_frame->max_locals = code->max_locals;
	stack_frame->max_stack = code->max_stack;

	code->current_frame = stack_frame;

	return 0;


}

int _parse_class_method_code(class_method* method,u2 name_index,CLASS** klass)
{
	method_code* code = (method_code*)malloc(sizeof(method_code));
	if(!code)
	{
		return -1;
	}
	code->attribute_name_index = name_index;
	CLASS_READ_U4(code->attribute_length,mem_start)
	CLASS_READ_U2(code->max_stack,mem_start)
	CLASS_READ_U2(code->max_locals,mem_start)
	CLASS_READ_U4(code->code_length,mem_start)
	u1* c = (u1*)malloc(sizeof(u1)*(code->code_length+1));
	if(!c)
	{
		free(code);
		return -1;
	}
	memset(c,'\0',code->code_length+1);
	memcpy(c,mem_start,code->code_length);
	mem_start += (code->code_length);
	code->code = c;

	if(-1 == init_method_stack_frame(code))//尝试了下，还是在这里就开辟栈帧容易点
	{
		free(c);
		free(code);
		return -1;
	}

	CLASS_READ_U2(code->exception_table_length,mem_start)
	if(code->exception_table_length > 0)
	{
		exception_table* ex_table = (exception_table*)malloc(sizeof(exception_table) * code->exception_table_length);
		if(!ex_table)
		{
			free(c);
			free(code);
			return -1;
		}
		u2 index;
		for(index=0;index<code->exception_table_length;index++)
		{
			if(_parse_class_method_ex_table(&(ex_table[index])))
			{
				return -1;
			}
		}

	}

	//attribute count
	CLASS_READ_U2(code->attributes_count,mem_start)
	if(code->attributes_count > 0)
	{
		u2 idx;
		for(idx=0;idx<code->attributes_count;idx++)
		{
			u2 name_index;
			CLASS_READ_U2(name_index,mem_start)

			char* name = ((constant_utf8_info*)(*klass)->cp_info[name_index].info)->bytes;
			if(0 == strcmp(name,"LineNumberTable"))
			{
				if(-1 == _pase_class_method_code_line_nubmer_table(code,name_index))
				{
					return -1;
				}

			}else if(0 == strcmp(name,"LocalVariableTable"))
			{
				//TODO
			}else if(0 == strcmp(name,"LocalVariableTypeTable"))
			{
				//TODO

			}else if(0 == strcmp(name," StackMapTable"))
			{
				//TODO
			}

		}

	}

	method->method_code = code;

	return 0;
}

int clinit_method(class_method* method)
{
	vm_pc.pc = method->method_code->code;
	if(-1 == interpreter_byte_code(method))
	{
		return -1;
	}
	return 0;
}

int _parse_class_method(class_method* method,CLASS** klass)
{
	CLASS_READ_U2(method->access_flags,mem_start)
	CLASS_READ_U2(method->name_index,mem_start)

	char* method_name = ((constant_utf8_info*)(*klass)->cp_info[method->name_index].info)->bytes;
	method->method_name = method_name;
	method->klass = (*klass);

	CLASS_READ_U2(method->descriptor_index,mem_start)
	CLASS_READ_U2(method->attributes_count,mem_start)
	if(method->attributes_count > 0)
	{
		u2 name_index;
		CLASS_READ_U2(name_index,mem_start)
		//must be CONSTANT_Utf8_info
		char* name = ((constant_utf8_info*)(*klass)->cp_info[name_index].info)->bytes;
		if(0 == strcmp(name,"Code"))
		{
			if(-1 == _parse_class_method_code(method,name_index,klass))
			{
				return -1;
			}
		}

	}

	if(0 == strcmp(method->method_name, "<clinit>"))
	{
		if(-1 == clinit_method(method))
		{
			return -1;
		}
	}

	return 0;
}

int parse_class_methods(CLASS** klass)
{
	CLASS_READ_U2((*klass)->methods_count,mem_start)
	if((*klass)->methods_count > 0)
	{
		class_method* methods = (class_method*)malloc(sizeof(class_method) * (*klass)->methods_count);
		if(!methods)
		{
			return -1;
		}
		u2 index;
		for(index=0;index<((*klass)->methods_count);index++)
		{
			if(-1 == _parse_class_method(&(methods[index]),klass))
			{
				return -1;
			}

		}
		(*klass)->methods = methods;
	}

	return 0;
}

int parse_class_source_file(CLASS* klass,u2 name_index)
{
	source_file* source = (source_file*)malloc(sizeof(source_file));
	if(!source)
	{
		return -1;
	}
	source->attribute_name_index = name_index;
	CLASS_READ_U4(source->attribute_length,mem_start)
	CLASS_READ_U2(source->sourcefile_index,mem_start)

	klass->source_file = source;
}

int parse_class_attr(CLASS** klass)
{
	CLASS_READ_U2((*klass)->attributes_count,mem_start)
	if((*klass)->attributes_count > 0)
	{
		u2 index;
		for(index=0;index<(*klass)->attributes_count;index++)
		{
			u2 name_index;
			CLASS_READ_U2(name_index,mem_start)
			//must be CONSTANT_Utf8_info
			char* name = ((constant_utf8_info*)(*klass)->cp_info[name_index].info)->bytes;
			if(0 == strcmp(name,"InnerClasses"))
			{
				//TODO
			}else if(0 == strcmp(name,"EnclosingMethod"))
			{
				//TODO
			}else if(0 == strcmp(name,"Synthetic"))
			{
				//TODO
			}else if(0 == strcmp(name,"Signature"))
			{
				//TODO
			}else if(0 == strcmp(name,"SourceFile"))
			{
				if(-1 == parse_class_source_file(klass,name_index))
				{
					return -1;
				}
			}else if(0 == strcmp(name,"SourceDebugExtension"))
			{
				//TODO
			}else if(0 == strcmp(name,"Deprecated"))
			{
				//TODO
			}else if(0 == strcmp(name,"RuntimeVisibleAnnotations"))
			{
				//TODO
			}else if(0 == strcmp(name,"RuntimeInvisibleAnnotations"))
			{
				//TODO
			}else if(0 == strcmp(name,"BootstrapMethods"))
			{
				//TODO
			}
		}
	}
}

int unmap(int size)
{
	if(-1 == munmap(class_start_mem,size))
	{
		return -1;
	}
	close(class_fd);
	return 0;
}

CLASS* parse_class_file(char* class_file)
{
	class_fd = open(class_file,O_RDONLY);
	if(!class_fd){
		printf("open class file %s failed",class_file);
		close(class_fd);
		return NULL;
	}

	struct stat statbuff;
	if(-1 == stat(class_file,&statbuff)){
		close(class_fd);
		return NULL;
	}

	int size = statbuff.st_size;

	mem_start = (char*)mmap(NULL,size,PROT_READ,MAP_SHARED,class_fd,0);
	if(!mem_start)
	{
		close(class_fd);
		printf("map failed");
		return NULL;
	}
	class_start_mem = mem_start;

	CLASS* klass;
	klass = (CLASS*)malloc(sizeof(CLASS));
	klass->super = NULL;
	klass->isArray = 0;
	klass->isInterface = 0;
	klass->primitive_type = -1;

	//parse magic
	if(-1 == parse_class_magic(&klass))
	{
		goto out;
	}
	//parse version
	if(-1 == parse_class_version(&klass))
	{
		goto out;
	}
	//parse constant_pool
	if(-1 == parse_constant_pool(&klass))
	{
		goto out;
	}
	//parse access flags
	CLASS_READ_U2(klass->access_flags,mem_start)
	if(klass->access_flags & ACC_INTERFACE)
	{
		klass->isInterface = 1;
	}
	//parse this_class
	CLASS_READ_U2(klass->this_class,mem_start)

	//get class name
	constant_class_info* cl = (constant_class_info*)((klass->cp_info[klass->this_class]).info);
	constant_utf8_info* utf = (constant_utf8_info*)((klass->cp_info[cl->name_index]).info);
	klass->class_name = utf->bytes;
	if(-1 == init_jvm_class_list(&klass,&class_list_head)){
		goto out;
	}
	//parse super_class
	CLASS_READ_U2(klass->super_class,mem_start)
	//parse interfaces
	if(-1 == parse_class_interfaces(&klass))
	{
		goto out;
	}
	//parse class field
	if(-1 == parse_class_fields(&klass))
	{
		goto out;
	}
	//parse_class_method
	if(-1 == parse_class_methods(&klass))
	{
		goto out;
	}

	//parse attr
	if(-1 == parse_class_attr(&klass))
	{
		goto out;
	}

	return klass;


out:
	free(klass);
	unmap(size);

}

CLASS* load_primitive_class(const char* classname)
{
	int len = strlen(classname);
	if(len>1){
		if((classname[len-1]=='I'&&classname[len-2]=='[') ||(classname[len-1]=='S'&&classname[len-2]=='[') || (classname[len-1]=='B'&&classname[len-2]=='[')
				|| (classname[len-1]=='L'&&classname[len-2]=='[') || (classname[len-1]=='F'&&classname[len-2]=='[') || (classname[len-1]=='D'&&classname[len-2]=='[')
				|| (classname[len-1]=='C'&&classname[len-2]=='[')){
			int dimensions = 0;
			char *p = classname;
			while(*p == '['){
				dimensions++;
				p++;
			}

			CLASS* cl = (CLASS*)malloc(sizeof(CLASS));
			cl->isArray = 1;
			cl->class_name = classname;
			cl->dimensions = dimensions;
			char c = *p;
			switch(c)
			{
			case 'I':
				cl->primitive_type = T_INT;
				break;
			case 'S':
				cl->primitive_type = T_SHORT;
				break;
			case 'B':
				cl->primitive_type = T_BYTE;
				break;
			case 'C':
				cl->primitive_type = T_CHAR;
				break;
			case 'L':
				cl->primitive_type = T_LONG;
				break;
			case 'F':
				cl->primitive_type = T_FLOAT;
				break;
			case 'D':
				cl->primitive_type = T_DOUBLE;
				break;
			default:
				cl->primitive_type = -1;
				break;

			}
			return cl;
		}
	}

	return NULL;
}

CLASS* jvm_load_class(const char* classpath,const char* classname)
{
	DIR* dir = opendir(classpath);
	if(!dir)
	{
		printf("opendir %s failed",classpath);
		closedir(dir);
		return NULL;
	}

	CLASS* ca = load_primitive_class(classname);
	if(ca){
		init_jvm_class_list(&ca,&class_list_head);
		return ca;
	}

	int dimensions = 0;
	char* p = classname;
	if(*classname =='['){
		p = (char*)malloc(sizeof(char)*(strlen(classname)+1));
		strcpy(p,classname);
		while(*p == '['){
			dimensions++;
			p++;
		}
		if(*p == 'L'){
			p++;
		}
	}
	int plen = strlen(p);
	if(*(p+plen-1)==';'){
		*(p+plen-1)='\0';
	}
	if(p != classname){
		CLASS* cs = find_class(&class_list_head,p);
		if(!cs){
			cs = jvm_load_class(classpath,p);
		}
		CLASS* cp = (CLASS*)malloc(sizeof(CLASS));
		memcpy(cp,cs,sizeof(CLASS));
		cp->class_name = classname;
		cp->isArray = 1;
		cp->dimensions = dimensions;
		return cp;
	}


	char temppath[1024];
	memset(temppath,'\0',1024);
	sprintf(temppath,"%s/%s.class",classpath,p);

	CLASS* klass = parse_class_file(temppath);
	if(!klass){
		printf("parse class file %s failed",temppath);
		closedir(dir);
	}
	closedir(dir);
	klass->class_name = classname;
	klass->dimensions = dimensions;

	//super
	CLASS* temp = klass;
	while(1)
	{
		u2 class_index = temp->super_class;
		u2 name_index = ((constant_class_info*)(temp->cp_info[class_index].info))->name_index;
		char* class_name = ((constant_utf8_info*)(temp->cp_info[name_index].info))->bytes;
		if(0==strcmp("java/lang/Object",class_name))
			break;
		CLASS* cl = find_class(&class_list_head,class_name);
		if(!cl)
		{
			cl = jvm_load_class(jvmargs->classpath,class_name);
		}
		temp->super = cl;
		temp = cl;

	}

	return klass;
}

CLASS* find_class(list* head,char* classname)
{
	list* p = head;
	p = p->next;
	for(;NULL != p;p=p->next){
		CLASS* klass = (CLASS*)(p->data);
		if(0 == strcmp(classname,(char*)(klass->class_name)))
		{
			return klass;
		}
	}

	return NULL;
}

class_field* find_class_field(list* head,char* classname,char* field_name)
{
	list* p = head;
	p = p->next;
	class_field* field;
	for(;NULL != p;p=p->next){
		CLASS* klass = (CLASS*)(p->data);
		if(0 == strcmp(classname,(char*)(klass->class_name)))
		{
			u2 idx;
			class_field* fields = klass->fields;
			for(idx=0;idx<klass->fields_count;idx++)
			{
				if(0 == strcmp(fields[idx].field_name,field_name))
				{
					field = (&(fields[idx]));
					return field;
				}
			}
		}
	}
	return NULL;
}

class_method* find_class_method(list* head,char* classname,char* method_name,char* desc_name)
{
	list* p = head;
	p = p->next;
	class_method* method;
	for(;NULL != p;p=p->next){
		CLASS* klass = (CLASS*)(p->data);
		if(0 == strcmp(classname,(char*)(klass->class_name)))
		{
			u2 idx;
			class_method* methods = klass->methods;
			for(idx=0;idx<klass->methods_count;idx++)
			{
				if(0 == strcmp(method_name,methods[idx].method_name))
				{
					method = (&(methods[idx]));
					if(0 != strcmp("main",method_name))
					{
						char* desc = ((constant_utf8_info*)(klass->cp_info[method->descriptor_index].info))->bytes;
						if(0 != strcmp(desc,desc_name))
							continue;
					}
					return method;
				}
			}
		}
	}
	return NULL;
}

/*int main(int args,char **argv)
{
	char *classpath = "/home/chengzhang/java_code_test";
	char *classname = "Field";
	if(!jvm_load_class(classpath,classname)){

	}
}*/
