#ifndef PARSE_H
#define PARSE_H
#include"global.h"
void parse_block();//�����ֳ���
void parse_const_dec();//��������˵��
void parse_var_dec();//��������˵��
enum type_type parse_proc_func_dec();//�������̻���˵��
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