#include"table.h"

/********************************************
*
* String Table
*
********************************************/
char str_tab[STR_TAB_SIZE];
int str_tab_idx = 0;

/*	将字符串存入字符号表中,返回所存字符串索引	*/
int store_str(char * str){
	int len;
	int index = str_tab_idx;
	len = strlen(str);

	if (str_tab_idx+len >= STR_TAB_SIZE-1){
		fatal(7);	/* 也许应该直接给出字符串	*/
		return -1;
	}

	strcpy(str_tab + str_tab_idx, str);
	str_tab_idx += (len + 1);
	return index;
}

/* 从字符串表中提取字符串 */
/* index:	字符串的索引 */
/* retrun:	字符串指针	*/
char * get_str(int index){
	if(index < 0||index > str_tab_idx){
		info(lineno, "字符串表引用错误！\n");
		return "";
	}
	return (char *) str_tab + index;
}

/************************************************/
/*												*/
/*					符号表						*/
/*												*/
/************************************************/

struct symbol_table_node symbol_table[TABLE_SIZE] = {{"__main",PROCEDURE, NO_TYPE, 0, 0, 0, 0}};

int symbol_tab_idx = 0;//0空出

int block_index[MAX_LEVEL+1] = {0};//每层子程序在符号表中的索引
int current_level = 1;	//level 0留给主函数，而主函数没有声明

int get_current_level()
{
	return current_level;
}
int get_current_block()
{
	return block_index[current_level];
}
/*int block_var_num = 0;//当前层的局部变量、常量数

int block_para_num = 0;//当前层的参数个数*/

char * identifier_name(int idx){
	return symbol_table[idx].name;
}
enum type_type identifier_type(int idx){
	return symbol_table[idx].type;
}
enum object_type identifier_object(int idx){
	return symbol_table[idx].object;
}
int identifier_size(int idx){
	return symbol_table[idx].size;
}
int identifier_value(int idx){
	return symbol_table[idx].value;
}
int identifier_level(int idx){
	return symbol_table[idx].level;
}
int identifier_offset(int idx){
	return symbol_table[idx].offset;
}

/*	从第max_level开始向上查找可见的identifier	*/
/*	返回索引,未找到返回0						*/
int find_identifier(char *name)
{
	int i = symbol_tab_idx;
	while(i >= block_index[current_level]){
		if(strcmp(name, identifier_name(i)) == 0)
			return i;
		i--;
	}
	while(i > 0){
		if(identifier_level(i) < current_level &&
			strcmp(name, identifier_name(i)) == 0)
			return i;
		i --;
	}
	return 0;
}
	

/*	最外层是否已经登记name,已则返回1，否则返回0	*/
int local_registed(char * name)
{
	int i;
	for(i = block_index[current_level]; i <= symbol_tab_idx; i++){
		if(strcmp(name, identifier_name(i)) == 0)
			return i;
	}
	return 0;
}
//向符号表中登入新标识符，及其所在层次
 void register_identifier(char * name){
	 if(symbol_tab_idx == SYMBOL_TABLE_SIZE)
		 fatal(1);
	 symbol_tab_idx ++;
	 strcpy(symbol_table[symbol_tab_idx].name, name);
	 symbol_table[symbol_tab_idx].level = current_level;
 }
 //计算函数过程有多少个参数
 int count_parameter(int func_idx){
	 int l;
	 int count = 0;
	 int i = func_idx + 1;
	 l = identifier_level(func_idx) + 1;
	 while((identifier_object(i) == VALUE_PARA || identifier_object(i) == VAR_PARA) &&
		 identifier_level(i) == l)
	 {
		 i++;
		 count++;
	 }
	 return count;
 }

 /*	注册过程函数 */
 /* 填入符号表，并更新block_index和current_level*/
 void register_block(char *name,enum object_type object){
	 register_identifier(name);
	 symbol_table[symbol_tab_idx].object = object;
	 if(current_level == MAX_LEVEL){
		 fatal(5);
		 return;
	 }
	 current_level ++;
	 block_index[current_level] = symbol_tab_idx;

 }

/*		子程序退栈		*/
/*	将符号表当前层退栈	*/
void pop_block(){
	int pop_end;
	int idx = symbol_tab_idx;
	pop_end = block_index[current_level];
	current_level--;	
	while(idx > pop_end){
		if((identifier_object(symbol_tab_idx) == VAR_PARA ||
			identifier_object(symbol_tab_idx) == VALUE_PARA)&&
			identifier_level(symbol_tab_idx) == current_level + 1)
			strcpy(symbol_table[idx].name, "____");
		else 
			symbol_tab_idx --;
		idx --;
	}
}

 void fix_block(enum type_type block_type){
	 int value_para_count = 0, var_size = 0, idx;
	 int block_idx = block_index[current_level];
	 symbol_table[block_idx].type = block_type;
	 symbol_table[block_idx].offset = 0;
	 idx = block_idx;
	 while(idx <= symbol_tab_idx){
		 if((identifier_object(idx) == VALUE_PARA || identifier_object(idx) == VAR_PARA) 
			 && identifier_level(idx) == current_level)
			value_para_count++;
		 if(identifier_object(idx) == VARIABLE)
			 var_size++;
		 if(identifier_object(idx) == ARRAY)
			 var_size += identifier_size(idx);
		 idx++;
	 }
	 symbol_table[block_idx].value = value_para_count;
	 symbol_table[block_idx].size = var_size;
 }

 //返回符号表中第idx个标识符应该分配的地址
 //VARIABLE,ARRAYC:从1开始
 //FUNCTION, PROCUDURE,CONSTANT, VAR_PARA:0
 //VALUE_PARA: 从1开始递减
 int get_offset(int idx){
	 int prev = idx -1;
	 int i;
	 int offset;
	 switch(symbol_table[idx].object){
	 case FUNCTION:
	 case PROCEDURE:		 
	 case CONSTANT:	 
		 return 0;
	 case VARIABLE:
	 case ARRAY:
		 offset = 1;
		 for(i = block_index[current_level]; i < idx; i++)
			 if(identifier_object(i) == VARIABLE || identifier_object(i) == ARRAY)
				 offset += identifier_size(i);
		 return offset;
	 case VAR_PARA:
	 case VALUE_PARA:
		 offset = 1;
		 for(i = block_index[current_level]; i < idx; i++)
			 if((identifier_object(i) == VALUE_PARA ||identifier_object(i) == VAR_PARA )&& identifier_level(i) == current_level)
				offset ++;
		 return offset;
	 default:
		 return 0;
	 }
 }


//将最近的count个标识符补上属性, size所占空间大小，若无空间，则为0，若为数组则是大小，变量1
void fix_identifier(int count, enum type_type type, enum object_type object, int size, int value){
	int idx;
	for(idx = symbol_tab_idx -count + 1;idx <= symbol_tab_idx; idx ++){
		symbol_table[idx].type = type;
		symbol_table[idx].object = object;
		if(object == ARRAY)
			symbol_table[idx].size = size;
		else if(object == CONSTANT || object == VAR_PARA)
			symbol_table[idx].size = 0;
		else 
			symbol_table[idx].size = 1;
		if(object == CONSTANT)
			symbol_table[idx].value = value;
		else
			symbol_table[idx].value = 0;
		symbol_table[idx].offset = get_offset(idx);
	}
}
void list_symbol_table(){
	int idx;
	printf("\n名称	种类	类型	层次	地址	值	大小\n");
	for(idx = 0; idx <= symbol_tab_idx; ++idx)
		printf("%s\t%2d\t%2d\t%2d\t%2d\t%2d\t%2d\n",symbol_table[idx].name,
		symbol_table[idx].object,symbol_table[idx].type, symbol_table[idx].level,
		symbol_table[idx].offset,symbol_table[idx].value,symbol_table[idx].size);

}


//产生一个新临时变量，返回地址
struct address new_temp(enum type_type type){
	struct address new_t;
	static int new_temp_i = 0;
	char temp_name[STR_SIZE];
	new_temp_i ++;
	sprintf(temp_name, ":tmp%d", new_temp_i);
	register_identifier(temp_name);

	fix_identifier(1, type, VARIABLE, 1, 0);
	
	new_t.pointer = symbol_tab_idx;
	new_t.type = VAR_ADDR;
	return new_t;
}


/************************************************
*												*
*					数组元素表					*
*												*
*************************************************/
//TODO: fixme
//not complete
struct array_element_node{
	int array;
	struct address array_addr;
	struct address index;
};

#define ARRAY_SIZE 100

struct array_element_node array_element_table[ARRAY_SIZE];
int array_table_idx = -1;
struct address get_array_addr(int idx){
	return array_element_table[idx].array_addr;
}
struct address get_array_index_addr(int idx){
	return array_element_table[idx].index;
}
struct address array_element(int array_idx, struct address offset_addr){
	struct address element_address;
	struct address array_addr;
	if(array_table_idx == ARRAY_SIZE){
		fatal(4);
		return;
	}
	array_table_idx ++;
	array_addr.type = VAR_ADDR;
	array_addr.pointer = array_idx;
	array_element_table[array_table_idx].array = array_idx;
	array_element_table[array_table_idx].index = offset_addr;
	array_element_table[array_table_idx].array_addr = array_addr;
	element_address.pointer = array_table_idx;
	element_address.type = ARRAY_ELEMENT_ADDR;
	return element_address;
}



/************************************************
*												*
*					code list					*
*												*
*************************************************/
struct code codelist[CODE_TABLE_SIZE];

int code_idx = -1;
int get_cur_code_idx(){
	return code_idx;
}
void clear_code_table(){
	code_idx = -1;
}
struct code get_ir_code(int idx){
	return codelist[idx];
}
static char *get_ins_name(enum instruction_type ins){
	switch(ins){
	case ADD_INS:return "ADD";	//	+	z=x+y
	case MINUS_INS:
		return "MINUS";//	-	z=x-y
	case DIV_INS:
		return "DIV";	//	/	z=x/y
	case MULT_INS:
		return "MULT";	//	*	z=x*y
	case NEG_INS:
		return "NEG";// z = -x
	case INC_INS:
		return "INC";//	加1	z++
	case DEC_INS:
		return "DEC";	//	减1	z--
	//case ARRAY_INS:
		//return "ARRAY";//	z=x[y]

	case ASSIGN_INS:
		return "ASSIGN";//	赋值z=x

	//case CMP_INS:
		//return "CMP";//	比较	flag = z-0
	case LABEL_INS:
		return "LABEL";//	标签	lable z
	case JMP_INS:
		return "JMP";//	无条件跳转	goto z
	case JMPE_INS:
		return "JMPE";//	相等跳转	if x=y goto z
	case JMPNE_INS:
		return "JMPNE";//	不相等跳转	if x <> y goto z
	case JMPL_INS:
		return "JMPL";//	小于跳转		if x < y goto z
	case JMPNL_INS:
		return "JMPNL";//	大于等于跳转 if x >= y goto z
	case JMPG_INS:
		return "JMPG";//	大于跳转		if x > y goto z
	case JMPNG_INS:
		return "JMPNG";//	小于等于跳转	if x <= y goto z
	//case IF_INS:
		//return "IF";		//	.IF
	//case ELSE_INS:
		//return "ELSE";//	.else
	//case ENDIF_INS:
		//return "ENDIF";//	.endif
	
	case VAR_PARA_INS:
		return "VAR_PARA";//变量参数		z
	case VALUE_PARA_INS:
		return "VALUE_PARA";//值参数		z
	case CALL_INS:
		return "CALL";//	调用过程函数	z 过程地址
	case RET_INS:
		return "RET";//	过程函数返回	z

	case READC_INS:
		return "READC";//	读入字符		char->z
	case READI_INS:
		return "READI";//	读入整数		int->z
	case WRITEC_INS:
		return "WRITEC";//	写字符			out << z
	case WRITEI_INS:
		return "WRITEI";	//	写整数			out << z
	case WRITES_INS:
		return "WRITES";
	case WRITEL_INS:
		return "WRITEL";
	case BLOCK_INS:
		return "BLOCK";
	case ERROR_INS:
		return "ERROR";
	//case ADDR_INS:
		return "ADDR";
	//case STORE_INS:
		return "STORE";
	default:
		return "越界";
	}
}//	写字符串		out	<< z

static char * get_element(struct address a){
	char buf[STR_SIZE];
	int i;
	switch(a.type){
	case VAR_ADDR:
		return identifier_name(a.pointer);
	case STRING_ADDR:
		return get_str(a.pointer);
	case VALUE_ADDR:
		sprintf(buf, "%d", a.pointer);
		i = store_str(buf);
		return get_str(i);
	case LABEL_ADDR:
		sprintf(buf, "%d", a.pointer);
		i = store_str(buf);
		return get_str(i);
	case ARRAY_ELEMENT_ADDR:
		sprintf(buf, "%s[%s]",identifier_name(array_element_table[a.pointer].array),
			get_element(array_element_table[a.pointer].index));
		i = store_str(buf);
		return get_str(i);
	default:
		return "NULL";
	}
};


void list_code()
{
	int i;
	printf("CODELIST:\n");
	printf("指令		x		y		z\n");
	for(i = 0; i <= code_idx; i++)
		printf("%s		%s		%s		%s\n", 
			get_ins_name(codelist[i].instruction), 
			get_element(codelist[i].x),
			get_element(codelist[i].y),
			get_element(codelist[i].z)
			);
}

void gen_code(enum instruction_type instrct,
struct address x,
struct address y,
struct address z){
	if(code_idx == CODE_TABLE_SIZE)
		fatal(6);
	code_idx++;
	codelist[code_idx].instruction = instrct;
	codelist[code_idx].x = x;
	codelist[code_idx].y = y;
	codelist[code_idx].z = z;
}

struct address new_label(){
	static int label_id = 0;
	struct address label_addr;
	label_addr.type = LABEL_ADDR;
	label_id ++;
	label_addr.pointer = label_id;
	return label_addr;
}
