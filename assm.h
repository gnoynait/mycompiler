#ifndef ASSM_H
#define ASSM_H

#include"global.h"
#include"table.h"

extern void init_assm(FILE * code_file);
//��code_table�е�start��ʼ��end������
//��ÿһ�����ɻ�����
extern void gen_assm(int end);
extern void assm_over();
#endif