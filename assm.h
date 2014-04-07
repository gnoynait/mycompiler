#ifndef ASSM_H
#define ASSM_H

#include"global.h"
#include"table.h"

extern void init_assm(FILE * code_file, int is_opt);
//��code_table�е�start��ʼ��end������
//��ÿһ�����ɻ�����
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