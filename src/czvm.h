/*
 * czvm.h
 *
 *
 *      Author: chengzhang
 */

#ifndef CZVM_H_
#define CZVM_H_

#include "type.h"
#include "classloader.h"
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define JVM_VERSION						0.01
#define JVM_BANNER						"@ chengzhang 2017"

#define JVM_LOG_PATH						"/log"

#define JVM_CLASS_MAGIC                 			0xcafebabe
#define STACK_ITEM_SIZE						sizeof(void*)

#define CONSTANT_Class					7
#define CONSTANT_Fieldref				9
#define CONSTANT_Methodref				10
#define CONSTANT_InterfaceMethodref			11
#define CONSTANT_String					8
#define CONSTANT_Integer				3
#define CONSTANT_Float					4
#define CONSTANT_Long					5
#define CONSTANT_Double					6
#define CONSTANT_NameAndType				12
#define CONSTANT_Utf8					1
#define CONSTANT_MethodHandle				15
#define CONSTANT_MethodType				16
#define CONSTANT_InvokeDynamic				18

#define T_BOOLEAN 	4
#define T_CHAR 	5
#define T_FLOAT 	6
#define T_DOUBLE 	7
#define T_BYTE 	8
#define T_SHORT 	9
#define T_INT 	10
#define T_LONG 	11


//class access flags
#define ACC_PUBLIC 0x0001
#define ACC_FINAL 0x0010
#define ACC_SUPER 0x0020
#define ACC_INTERFACE 0x0200
#define ACC_ABSTRACT 0x0400
#define ACC_SYNTHETIC 0x1000
#define ACC_ANNOTATION 0x2000
#define ACC_ENUM 0x4000


typedef struct jvmArgs
{
	char classpath[1024];
	char log_path[1024];
}jvmArgs;
jvmArgs *jvmargs;

/////////////////////////////////////////jvm class struct 参照jvm官方文档定义的数据结构////////////////////////////////////////////////////////////////////////////////

typedef struct cp_info
{
	u2 index;
    u1 tag;
    u1* info;
}constant_pool_info;

typedef struct CONSTANT_Fieldref_info {
    u2 class_index;
    u2 name_and_type_index;
}constant_field_info;

typedef struct CONSTANT_Methodref_info {
    u2 class_index;
    u2 name_and_type_index;
}constant_method_info;

typedef struct CONSTANT_InterfaceMethodref_info {
    u2 class_index;
    u2 name_and_type_index;
}constant_interface_info;

typedef struct CONSTANT_Class_info {
    u2 name_index;
}constant_class_info;

typedef struct CONSTANT_Utf8_info {
    u2 length;
    u1* bytes;
}constant_utf8_info;

typedef struct CONSTANT_NameAndType_info {
    u2 name_index;
    u2 descriptor_index;
}constant_name_type_info;

typedef struct CONSTANT_String_info {
    u2 string_index;
}constant_string_info;

typedef struct CONSTANT_Integer_info {
    u4 bytes;
}constant_integer_info;

typedef struct CONSTANT_Float_info {
    u4 bytes;
}constant_float_info;

typedef struct CONSTANT_Long_info {
    u4 high_bytes;
    u4 low_bytes;
}constant_long_info;

typedef struct CONSTANT_Double_info {
    u4 high_bytes;
    u4 low_bytes;
}constant_double_info;

typedef struct CONSTANT_MethodHandle_info {
    u1 reference_kind;
    u2 reference_index;
}constant_methodHandle_info;

typedef struct CONSTANT_MethodType_info {
    u2 descriptor_index;
}constant_methodtype_info;

typedef struct CONSTANT_InvokeDynamic_info {
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
}constant_invoke_dynamic_info;

typedef struct ConstantValue_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 constantvalue_index;
}field_constantValue;

typedef struct Synthetic_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
}synthetic;

typedef struct Signature_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 signature_index;
}signature;

typedef struct Deprecated_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
}deprecated;

typedef struct field_info {
    u2             access_flags;
    u2             name_index;
    u2             descriptor_index;
    u2             attributes_count;
    field_constantValue* constantValue_attribute;
    synthetic* synthetic_attribute;
    signature* signature_attribute;
    deprecated* deprecated_attribute;

    u1* field_name;
    long field_value;//基本类型存储的之基本类型的值，非基本类型存储的是jvm_obj地址
}class_field;

typedef struct exception_table{
	u2 start_pc;
	u2 end_pc;
	u2 handler_pc;
	u2 catch_type;
}exception_table;

typedef struct line_number_table{
	u2 start_pc;
	u2 line_number;
}line_number_table;

typedef struct LineNumberTable_attribute{
	u2 attribute_name_index;
	u4 attribute_length;
	u2 line_number_table_length;
	line_number_table* line_number_table;

}LineNumberTable;

typedef struct local_variable_table{
	u2 start_pc;
	u2 length;
	u2 name_index;
	u2 descriptor_index;
	u2 index;
}local_variable_table;

typedef struct LocalVariableTable_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 local_variable_table_length;
    local_variable_table* local_variable_table;
}LocalVariableTable;

typedef struct local_variable_type_table{
	u2 start_pc;
	u2 length;
	u2 name_index;
	u2 signature_index;
	u2 index;
}local_variable_type_table;

typedef struct LocalVariableTypeTable_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 local_variable_type_table_length;
    local_variable_type_table* local_variable_type_table;
}LocalVariableTypeTable;

typedef struct verification_type_info {
	union{
		struct Top_variable_info{
			u1 tag;// = ITEM_Top; /* 0 */
		}a;
		struct Integer_variable_info{
			u1 tag;// = ITEM_Integer; /* 1 */
		}b;
		struct Float_variable_info{
			u1 tag;// = ITEM_Float; /* 2 */
		}c;
		struct Long_variable_info{
			u1 tag;// = ITEM_Long; /* 4 */
		}d;
		struct Double_variable_info{
			u1 tag;// = ITEM_Double; /* 3 */
		}e;
		struct Null_variable_info {
		    u1 tag;// = ITEM_Null; /* 5 */
		}f;
		struct UninitializedThis_variable_info{
			u1 tag;// = ITEM_UninitializedThis; /* 6 */
		}g;
		struct Object_variable_info{
			u1 tag;// = ITEM_Object; /* 7 */
			u2 cpool_index;
		}h;
		struct Uninitialized_variable_info{
			u1 tag;// = ITEM_Uninitialized /* 8 */
			u2 offset;
		}i;
	};
	u1 tag;

}verification_type_info;

typedef struct stack_map_frame {
	union{
		struct same_frame {
			    u1 frame_type; /* 0-63 */
			}a;
		struct same_locals_1_stack_item_frame{
			u1 frame_type; /* 64-127 */
			verification_type_info stack[1];
		}b;
		struct same_locals_1_stack_item_frame_extended {
			u1 frame_type; /* 247 */
			u2 offset_delta;
			verification_type_info stack[1];
		}c;
		struct chop_frame {
			u1 frame_type; /* 248-250 */
			u2 offset_delta;
		}d;
		struct same_frame_extended {
			u1 frame_type; /* 251 */
			u2 offset_delta;
		}e;
		struct append_frame {
			u1 frame_type; /* 252-254 */
			u2 offset_delta;
			verification_type_info* locals;
		}f;
		struct full_frame {
			u1 frame_type; /* 255 */
			u2 offset_delta;
			u2 number_of_locals;
			verification_type_info* locals;
			u2 number_of_stack_items;
			verification_type_info* stack;
		}g;
	};
	u1 frame_type;
	u1 stack_num;
	u1 locals_num;
	u1 offset_delta;
}stack_map_frame;

typedef struct StackMapTable_attribute {
    u2              attribute_name_index;
    u4              attribute_length;
    u2              number_of_entries;
    stack_map_frame* entries;
}StackMapTable;

typedef struct jvm_stack_object{//记录在栈中分配的对象,处理ref_count
	void* obj;
	struct jvm_stack_object* next;
}jvm_stack_obj_list;

typedef struct jvm_stack_frame{//运行是栈帧结构
	u1* local_variables;//本地变量表
	u1* operand_stacks;//操作数栈
	u1* return_address;//返回地址
	u4 operand_offset;//操作数栈偏移,相当与数组下标，记录下一个可以存放的位置
	u2 max_stack;
	u2 max_locals;

	struct jvm_stack_frame* prev;//指向前一个栈帧,全c实现，没有c++现成的数据结构支持，用链表方式实现比较方便

	jvm_stack_obj_list* list;//栈帧中分配的对象 new关键字

}jvm_stack_frame;

typedef struct Code_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    u2 exception_table_length;
    exception_table* ex_table;
    u2 attributes_count;
    LineNumberTable* lineNumberTable;
    LocalVariableTable*  localVariableTable;
    LocalVariableTypeTable*  localVariableTypeTable;
    StackMapTable*		 stackMapTable;

    jvm_stack_frame* current_frame;//指令所在的栈桢

}method_code;

typedef struct Exceptions_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 number_of_exceptions;
    u2* exception_index_table;
}method_exception;


typedef struct class_method{
	u2             access_flags;
	u2             name_index;
	u2             descriptor_index;
	u2             attributes_count;
	method_code* 	method_code;
	method_exception* method_exception;
	synthetic* method_synthetic;
	signature* method_signature;
	deprecated* method_deprecated;
	//RuntimeVisibleAnnotations  //TODO
	//RuntimeInvisibleAnnotations //TODO
	// RuntimeVisibleParameterAnnotations //TODO
	//RuntimeInvisibleParameterAnnotations //TODO
	//AnnotationDefault //TODO

	u1* method_name;
	struct jvmClass* klass;


}class_method;

typedef struct inner_classes{
	u2 inner_class_info_index;
	u2 outer_class_info_index;
	u2 inner_name_index;
	u2 inner_class_access_flags;
}inner_classes;

typedef struct InnerClasses_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 number_of_classes;
    inner_classes* inner_classes;
}inner_class_attr;

typedef struct EnclosingMethod_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 class_index;
    u2 method_index;
}enclosing_method_attr;

typedef struct SourceFile_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 sourcefile_index;
}source_file;

typedef struct SourceDebugExtension_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u1* debug_extension;
}source_debug_extension_attr;

typedef struct jvmClass
{
	u4             magic;
    u2             minor_version;
    u2             major_version;
    u2             constant_pool_count;
    constant_pool_info*	cp_info;
    u2             access_flags;
    u2             this_class;
    u2             super_class;
    u2             interfaces_count;
    u2*            interfaces;
    u2             fields_count;
    class_field*    fields;
    u2             methods_count;
    class_method*    methods;
    u2             attributes_count;
    inner_class_attr* inner_class_attr;
    enclosing_method_attr* enclosing_method_attr;
    synthetic* synthetic;
    signature* signature;
    source_file* source_file;
    source_debug_extension_attr* source_debug_extension_attr;
    deprecated* deprecated;

    u1* class_name;

    //RuntimeVisibleAnnotations TODO
    //RuntimeInvisibleAnnotations TODO
    //BootstrapMethods TODO

    struct jvmClass* super;//父类
    int isArray;//数组标志
    int dimensions;//数组维度
    int isInterface;//是否为接口
    int primitive_type;//表示的基本类型

}CLASS;

typedef struct JVM_OBJECT_FIELD_VALUE{//对象的field值表示
	char* field_name;
	char* field_desc;
	long value;//基本类型存储值，其他为jvm_obj地址
	struct JVM_OBJECT_FIELD_VALUE* next;
}obj_field_value;

typedef struct JVM_OBJECT{
	int ref_count;
	int age;//后续可能有用，先保留着
	CLASS* class;
	long value;//基本类型存储值，其他为jvm_obj地址,此处的作用一是用来表示数组(根据指令执行的方式，使用这样的数据结构表示数组),而是用来表示值
	int isArray;
	int type;//T_BYTE,T_INT.......
	obj_field_value* field_values;
}jvm_obj;

//function
int jvm_args_init(char** agrv);
int jvm_init(jvmArgs *jvmargs,char *classname);
CLASS* jvm_load_class(const char* classpath,const char* classname);
int init_jvm_class_list(CLASS** klass,list* list);


/////////////////////////////////////runtime required///////////////////////////////////////////
typedef struct jvm_pc{
	u1* pc;
}jvm_pc;
jvm_pc vm_pc;//pc for count

typedef struct interpreter_env{//当前栈桢对应的常量池信息
	constant_pool_info* cp_info;
	struct interpreter_env* prev;
}interpreter_env;


jvm_stack_frame* cur_stack_frame;
interpreter_env* cur_interpreter_env;

int jvm_stack_depth;

#endif /* CZVM_H_ */
