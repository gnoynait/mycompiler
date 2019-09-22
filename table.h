#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include"global.h"
#include"util.h"
//字符串保存区大小
#define STAB_SIZE 600



#define KEY_NUM 20
#define TABLE_SIZE 200
#define CODE_TABLE_SIZE 200


extern char * identifier_name(int idx);
extern enum type_type identifier_type(int idx);
extern enum object_type identifier_object(int idx);
extern int identifier_size(int idx);
extern int identifier_value(int idx);
extern int identifier_level(int idx);
extern int identifier_offset(int idx);

extern int find_identifier(char *name);

//extern char stab[STAB_SIZE];
extern int get_current_level();

//向符号表中登入新标识符，及其所在层次
extern void register_identifier(char * name);
extern int get_current_block();
/*//从后向前查找符号，直到0，返回下标
extern int find_identifier(char * name);*/
/* 进入一个分程序前调用*/
extern void clear_code_table();
/*返回当前中间代码下标*/
extern int get_cur_code_idx();
/*返回下标为idx的中间代码*/
extern struct code get_ir_code(int idx);
extern void gen_code(enum instruction_type instruction,struct address x,struct address y,struct address z);
//检查标识符在本层是否已经注册，如果在函数和过程中，函数过程名也要检查
extern int local_registed(char * name);
/*//将常量的类型，值补上
extern void fix_const(enum type_type cost_type, int value);*/
//将最近的count个标识符补上属性, size为数组的大小，如果不是数组则为0
extern void fix_identifier(int count, enum type_type type, enum object_type object, int size, int value);
//将子程序信息补全
extern void register_block(char *name, enum object_type object);
extern void fix_block(enum type_type);
extern int count_parameter(int func_idx);
//打印符号表
extern void list_symbol_table();

//返回最近一个子程序的下标
//extern int find_latest_block(int level);
//子程序退栈
extern void pop_block();
//清除level层次的临时变量
extern void pop_temp(int level);
//产生一个新临时变量，返回地址
extern struct address new_temp(enum type_type type);
//产生一个新的数组元素地址，array_idx是数组名在符号表中的下标，index是下标的临时变量
extern struct address array_element(int array_idx, struct address index);
extern struct address get_array_addr(int idx);
extern struct address get_array_index_addr(int idx);
extern enum type_type temp_type(int index);
extern void list_code();
//int count_temp(int level);
extern struct address new_label();
extern int store_str(char * str);
extern char * get_str(int str_idx);
#endif // TABLE_H_INCLUDED
