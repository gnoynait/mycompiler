#ifndef ASSM_H
#define ASSM_H

#include"global.h"
#include"table.h"

extern void init_assm(FILE * code_file, int is_opt);
//从code_table中的start开始到end结束，
//对每一行生成汇编代码
extern void gen_assm(int end);
extern void assm_over();
/***************************/

enum register_type{
	EAX,
	EBX,
	ECX,
	EDX,
	EDI,
	ESI,
	EBP,
	ESP,
	NO_REG
};
struct register_map_type{
	enum register_type reg;
	struct address addr;
};
#endif