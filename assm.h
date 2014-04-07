#ifndef ASSM_H
#define ASSM_H

#include"global.h"
#include"table.h"

extern void init_assm(FILE * code_file);
//从code_table中的start开始到end结束，
//对每一行生成汇编代码
extern void gen_assm(int end);
extern void assm_over();
#endif