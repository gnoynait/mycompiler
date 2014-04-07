#ifndef PARSE_H
#define PARSE_H
#include"global.h"
void parse_block();//解析分程序
void parse_const_dec();//解析常量说明
void parse_var_dec();//解析变量说明
enum type_type parse_proc_func_dec();//解析过程或函数说明
void parse_if_statement();
struct address parse_factor();
struct address parse_term();
void parse_case_statement();
void parse_multi_statement();
void parse_read_statement();
void parse_write_statement();
void parse_for_statement();
void parse_statement();
extern void parse_programe();
struct address parse_expression();
#endif