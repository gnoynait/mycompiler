#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include"global.h"
#include"util.h"
//�ַ�����������С
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

//����ű��е����±�ʶ�����������ڲ��
extern void register_identifier(char * name);
extern int get_current_block();
/*//�Ӻ���ǰ���ҷ��ţ�ֱ��0�������±�
extern int find_identifier(char * name);*/
/* ����һ���ֳ���ǰ����*/
extern void clear_code_table();
/*���ص�ǰ�м�����±�*/
extern int get_cur_code_idx();
/*�����±�Ϊidx���м����*/
extern struct code get_ir_code(int idx);
extern void gen_code(enum instruction_type instruction,struct address x,struct address y,struct address z);
//����ʶ���ڱ����Ƿ��Ѿ�ע�ᣬ����ں����͹����У�����������ҲҪ���
extern int local_registed(char * name);
/*//�����������ͣ�ֵ����
extern void fix_const(enum type_type cost_type, int value);*/
//�������count����ʶ����������, sizeΪ����Ĵ�С���������������Ϊ0
extern void fix_identifier(int count, enum type_type type, enum object_type object, int size, int value);
//���ӳ�����Ϣ��ȫ
extern void register_block(char *name, enum object_type object);
extern void fix_block(enum type_type);
extern int count_parameter(int func_idx);
//��ӡ���ű�
extern void list_symbol_table();

//�������һ���ӳ�����±�
//extern int find_latest_block(int level);
//�ӳ�����ջ
extern void pop_block();
//���level��ε���ʱ����
extern void pop_temp(int level);
//����һ������ʱ���������ص�ַ
extern struct address new_temp(enum type_type type);
//����һ���µ�����Ԫ�ص�ַ��array_idx���������ڷ��ű��е��±꣬index���±����ʱ����
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
