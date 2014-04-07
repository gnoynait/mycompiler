#ifndef GLOBAL_H
#define GLOBAL_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define DEBUG 1
/* 标识符最大长度 */
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

/*集合类型, 用于跳转 */
typedef int* set_type;


enum boolean{
	TRUE,
	FALSE
};

//标识符的类别
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

	INTEGER_CONST_SYM,	//	整型常量
	CHAR_CONST_SYM,		//	字符常量
	STR_CONST_SYM,		//	字符串常量

	IDENTIFIER_SYM		//	标识
};



/* 符号表中的类型 */
enum object_type{
	PROCEDURE,	//过程
	FUNCTION,	//函数
	VAR_PARA,	//变量参数
	VALUE_PARA,	//值参
	CONSTANT,	//常量
	VARIABLE,	//变量
	ARRAY       //数组
};

/* basic types */
enum type_type{
	NO_TYPE,	//无类型
	INT_TYPE,	//整型
	CHAR_TYPE	//字符型
};


#define MAX_NAME_LEN 30

struct symbol_table_node{
	char name[MAX_NAME_LEN];//标识符名称
	enum object_type object;//种类，包括常量，变量，过程，函数，数组，VAR_PARA, VALUE_PARA
	enum type_type type;//类型，包括整型，字符型
	//int reference;//用于引用外表
	//int is_normal;//是否是变量形参
	//int previous;//同一层次中标识符最后一个地址
	int level;//层次数
	int offset;//相对 于基地址的偏移量
	int value;//整型常量和字符型常量的值,过程函数参数个数
	int size;//数组的长度，函数过程的空间大小
};



/*程序表结点*/
struct block_table_node{
	int last;
	int lastpar;
	int psize;
	int vsize;
};

//四元式指令
enum instruction_type{
	ADD_INS,	//	+	z=x+y
	MINUS_INS,	//	-	z=x-y
	DIV_INS,	//	/	z=x/y
	MULT_INS,	//	*	z=x*y
	NEG_INS,	// z = -x
	INC_INS,	//	加1	z++
	DEC_INS,	//	减1	z--
	//ARRAY_INS,	//	z=x[y]

	ASSIGN_INS,	//	赋值z=x

	//CMP_INS,	//	比较	flag = z-0
	LABEL_INS,	//	标签	lable z
	JMP_INS,	//	无条件跳转	goto z
	JMPE_INS,	//	相等跳转	if x=y goto z
	JMPNE_INS,	//	不相等跳转	if x <> y goto z
	JMPL_INS,	//	小于跳转		if x < y goto z
	JMPNL_INS,	//	大于等于跳转 if x >= y goto z
	JMPG_INS,	//	大于跳转		if x > y goto z
	JMPNG_INS,	//	小于等于跳转	if x <= y goto z
	//IF_INS,		//	.IF
	//ELSE_INS,	//	.else
	//ENDIF_INS,	//	.endif
	
	VAR_PARA_INS,	//变量参数		z
	VALUE_PARA_INS,	//值参数		z
	CALL_INS,	//	调用过程函数	x过程地址, z返回值地址
	RET_INS,	//	过程函数返回	z

	READC_INS,	//	读入字符		char->z
	READI_INS,	//	读入整数		int->z
	/*READL_INS,	//  Read a new line character.*/
	WRITEC_INS,	//	写字符			out << z
	WRITEI_INS,	//	写整数			out << z
	WRITES_INS,	//	写字符串		out	<< z
	WRITEL_INS,	//	写换行符

	BLOCK_INS,	//进入子程序

	ERROR_INS,		//无操作，出错
	//ADDR_INS,		//取地址 取 x[y]的地址存到z
	//STORE_INS		//将x 的内容存到地址z
};


//四元式地址类型
enum address_type{
	EMPTY_ADDR,	//空	
	VAR_ADDR,	//符号表标识符地址
	VALUE_ADDR,	//值
	LABEL_ADDR,	//标签
	STRING_ADDR,	//字符串
	ARRAY_ELEMENT_ADDR //数组元素地址
};


//四元式中地址
struct address{
	enum address_type type;
	int pointer;
};

//四元式
struct code{
	enum instruction_type instruction;
	struct address x;
	struct address y;
	struct address z;
};

struct temp_node{
	enum type_type type;/* CHAR_TYEP or INTEGER_TYPE */
	int level;			/* 层次 */
	int offset;			/* 距过程函数的距离 */
};

extern FILE * source_file;	/* 源程序文件 */
//extern FILE * log_file;		/* 日志文件 */
extern FILE * code_file;	/* 输出汇编程序文件 */

extern int lineno;			/* 当前行号 */


#endif