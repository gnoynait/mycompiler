#ifndef GLOBAL_H
#define GLOBAL_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define DEBUG 1
/* ��ʶ����󳤶� */
#define ID_SIZE 20
//max integer.
#define STR_SIZE 80
#define INTEGER_MAX 32767 //2**15-1
#define BUFFER_SIZE     150
#define DIGITS_SIZE     10
#define SYMBOL_TABLE_SIZE      100
#define STR_TAB_SIZE 6000
#define BLOCK_TABLE_SIZE    20
#define CASE_NUM       30
#define CODE_LEN   600
#define MAX_LEVEL       7
#define ERROR_MAX       58
#define INSTRUCTION_NUM    63

/*��������, ������ת */
typedef int* set_type;


enum boolean{
	TRUE,
	FALSE
};

//��ʶ�������
#define SYMBOL_NUM 43
enum symbol_type{
	PLUS_SYM,			//	+
	MINUS_SYM,			//	-
	MULT_SYM,			//	*
	DIV_SYM,			//	/

	EQL_SYM,			//	=
	LSS_SYM,			//	<
	NEQL_SYM,			//	<>
	LEQ_SYM,			//	<=
	GTR_SYM,			//	>
	GEQ_SYM,			//	>=


	LROUND_SYM,			//	(
	RROUND_SYM,			//	)
	LSQUARE_SYM,		//	[
	RSQUARE_SYM,		//	]

	COMMA_SYM,			//	,
	SEMICOLON_SYM,		//	;
	PERIOD_SYM,			//	.

	COLON_SYM,			//	:
	BECOMES_SYM,		//	:=

	CONST_SYM,			//	const
	VAR_SYM,			//	var
	
	CHAR_SYM,			//	char
	INTEGER_SYM,		//	integer
	ARRAY_SYM,			//	array
	FUNC_SYM,			//	function
	PROC_SYM,			//	procedure

	BEGIN_SYM,			//	begin
	END_SYM,			//	end

	IF_SYM,				//	if
	THEN_SYM,			//	then
	ELSE_SYM,			//	else

	FOR_SYM,			//	for
	TO_SYM,				//	to
	DOWNTO_SYM,			//	downto
	DO_SYM,				//	do
	CASE_SYM,			//	case
	OF_SYM,				//	of
	
	READ_SYM,			//	read
	WRITE_SYM,			//	write

	INTEGER_CONST_SYM,	//	���ͳ���
	CHAR_CONST_SYM,		//	�ַ�����
	STR_CONST_SYM,		//	�ַ�������

	IDENTIFIER_SYM		//	��ʶ
};



/* ���ű��е����� */
enum object_type{
	PROCEDURE,	//����
	FUNCTION,	//����
	VAR_PARA,	//��������
	VALUE_PARA,	//ֵ��
	CONSTANT,	//����
	VARIABLE,	//����
	ARRAY       //����
};

/* basic types */
enum type_type{
	NO_TYPE,	//������
	INT_TYPE,	//����
	CHAR_TYPE	//�ַ���
};


#define MAX_NAME_LEN 30

struct symbol_table_node{
	char name[MAX_NAME_LEN];//��ʶ������
	enum object_type object;//���࣬�������������������̣����������飬VAR_PARA, VALUE_PARA
	enum type_type type;//���ͣ��������ͣ��ַ���
	//int reference;//�����������
	//int is_normal;//�Ƿ��Ǳ����β�
	//int previous;//ͬһ����б�ʶ�����һ����ַ
	int level;//�����
	int offset;//��� �ڻ���ַ��ƫ����
	int value;//���ͳ������ַ��ͳ�����ֵ,���̺�����������
	int size;//����ĳ��ȣ��������̵Ŀռ��С
};



/*�������*/
struct block_table_node{
	int last;
	int lastpar;
	int psize;
	int vsize;
};

//��Ԫʽָ��
enum instruction_type{
	ADD_INS,	//	+	z=x+y
	MINUS_INS,	//	-	z=x-y
	DIV_INS,	//	/	z=x/y
	MULT_INS,	//	*	z=x*y
	NEG_INS,	// z = -x
	INC_INS,	//	��1	z++
	DEC_INS,	//	��1	z--
	//ARRAY_INS,	//	z=x[y]

	ASSIGN_INS,	//	��ֵz=x

	//CMP_INS,	//	�Ƚ�	flag = z-0
	LABEL_INS,	//	��ǩ	lable z
	JMP_INS,	//	��������ת	goto z
	JMPE_INS,	//	�����ת	if x=y goto z
	JMPNE_INS,	//	�������ת	if x <> y goto z
	JMPL_INS,	//	С����ת		if x < y goto z
	JMPNL_INS,	//	���ڵ�����ת if x >= y goto z
	JMPG_INS,	//	������ת		if x > y goto z
	JMPNG_INS,	//	С�ڵ�����ת	if x <= y goto z
	//IF_INS,		//	.IF
	//ELSE_INS,	//	.else
	//ENDIF_INS,	//	.endif
	
	VAR_PARA_INS,	//��������		z
	VALUE_PARA_INS,	//ֵ����		z
	CALL_INS,	//	���ù��̺���	x���̵�ַ, z����ֵ��ַ
	RET_INS,	//	���̺�������	z

	READC_INS,	//	�����ַ�		char->z
	READI_INS,	//	��������		int->z
	/*READL_INS,	//  Read a new line character.*/
	WRITEC_INS,	//	д�ַ�			out << z
	WRITEI_INS,	//	д����			out << z
	WRITES_INS,	//	д�ַ���		out	<< z
	WRITEL_INS,	//	д���з�

	BLOCK_INS,	//�����ӳ���

	ERROR_INS,		//�޲���������
	//ADDR_INS,		//ȡ��ַ ȡ x[y]�ĵ�ַ�浽z
	//STORE_INS		//��x �����ݴ浽��ַz
};


//��Ԫʽ��ַ����
enum address_type{
	EMPTY_ADDR,	//��	
	VAR_ADDR,	//���ű��ʶ����ַ
	VALUE_ADDR,	//ֵ
	LABEL_ADDR,	//��ǩ
	STRING_ADDR,	//�ַ���
	ARRAY_ELEMENT_ADDR //����Ԫ�ص�ַ
};


//��Ԫʽ�е�ַ
struct address{
	enum address_type type;
	int pointer;
};

//��Ԫʽ
struct code{
	enum instruction_type instruction;
	struct address x;
	struct address y;
	struct address z;
};

struct temp_node{
	enum type_type type;/* CHAR_TYEP or INTEGER_TYPE */
	int level;			/* ��� */
	int offset;			/* ����̺����ľ��� */
};

extern FILE * source_file;	/* Դ�����ļ� */
//extern FILE * log_file;		/* ��־�ļ� */
extern FILE * code_file;	/* ����������ļ� */

extern int lineno;			/* ��ǰ�к� */


#endif