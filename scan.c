#include"global.h"
#include"scan.h"
#include"table.h"
#include"util.h"
#include<stdlib.h>

char identifier[ID_SIZE];    //cur identifier
char ch = ' ';            //current char
enum symbol_type symbol;         //current symbol
char str_buffer[BUFFER_SIZE];
int num;
#define LINE_MAX 121
static char line[LINE_MAX];
static int cc = 0;         //char count
static int ll = 0;         //line length
FILE * source_file;
int end_of_file = 0;

#define RESERVE_NUM 20		//保留字个数
//保留字名
char * reserve_name[RESERVE_NUM]={
	"const",
	"var",
	"char",
	"integer",
	"array",
	"function",
	"procedure",
	"begin",
	"end",
	"if",
	"then",
	"else",
	"for",
	"to",
	"downto",
	"do",
	"case",
	"of",
	"read",
	"write"
};
//保留字标识符
enum symbol_type reserve_symbol[RESERVE_NUM]={
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
};



enum symbol_type ch_symbol(int c){
	switch(c){
	case '+':
		return PLUS_SYM;
	case '-':
		return MINUS_SYM;
	case '*':
		return MULT_SYM;
	case '/':
		return DIV_SYM;
	case '=':
		return EQL_SYM;
	case '(':
		return LROUND_SYM;
	case ')': 
		return RROUND_SYM;
	case '[':
		return LSQUARE_SYM;
	case ']':
		return RSQUARE_SYM;
	case ',':
		return COMMA_SYM;
	case ';':
		return SEMICOLON_SYM;
	case '.':
		return PERIOD_SYM;
	default:	//失败，返回END_SYM作为出错特征
		return END_SYM;
	}
}


static int next_ch()
{
	/*if(end_of_file){
		info(lineno, "程序不完整");
		fclose(source_file);
		fclose(code_file);
		fclose(source_file);
		fclose(code_file);
		free(const_first);
		free(factor_first);
		free(statement_first);
		free(block_first);
		exit(-1);
	}*/
	if(cc == ll){
		if (feof(source_file)){
			info(lineno, "program incomplete.\n");
			return -1;
		}
		ll=0;
		cc=0;
		ch = ' ';
		while (ch != '\n'){
			if (EOF == fscanf(source_file,"%c", &ch)){
				line[ll] = '\0';
				end_of_file = 1;
				break;
			}
			printf("%c", ch);
			//fprintf(log_file, "%c", ch);
			line[ll] = ch;
			ll++;
		}
		++lineno;
	}
	ch = line[cc];
	cc++;
	return 0;
}


int next_sym()
{
	int k;
	/* 忽略空格、换行和TAB */
	while (isspace(ch)){
		next_ch();
	}
	if (isalpha(ch)){			/* 名字或保留字以alpha开头 */
		k = 0;
		symbol = IDENTIFIER_SYM;
		do {
			if(k < ID_SIZE-1){
				identifier[k] = ch;
				k++;
			}
			next_ch();
		} while (isalnum(ch));
		identifier[k] = '\0';

		for(k = 0; k < RESERVE_NUM; k++){
			if(strcmp(identifier, reserve_name[k]) == 0){
				symbol = reserve_symbol[k];
				break;
			}
		}
	}
	else if (isdigit(ch)){			/* 检测是否为数字：以0..9开头 */
		num = 0;
		symbol = INTEGER_CONST_SYM;
		do {
			num = 10*num + ch - '0';
			next_ch();
		} while (isdigit(ch)); /* 获取数字的值 */
		
		if (num > INTEGER_MAX || num < 0){
			error(lineno, 21);		//数值太大
			num = 0;
		}
	}
	else if (ch == ':'){		/* 检测赋值符号 */
		next_ch();
		if (ch == '='){
			symbol = BECOMES_SYM;
			next_ch();
		}
		else {
			symbol = COLON_SYM;
		}
	}
	else if (ch == '<') {		/* 检测小于或小于等于符号 */
		next_ch();
		if (ch == '='){
			symbol = LEQ_SYM;
			next_ch();
		}
		else if(ch =='>'){
			symbol = NEQL_SYM;
			next_ch();
		}
		else symbol = LSS_SYM;
	}
	else if (ch=='>'){		/* 检测大于或大于等于符号 */
		next_ch();
		if (ch == '='){
			symbol = GEQ_SYM;
			next_ch();
		}
		else symbol = GTR_SYM;
	}
	else if (ch == '"'){
		k=0;
		do{
			next_ch();
			if(!(isalnum(ch)|| isspace(ch) || ch == '"')){
				error(lineno, 59);
			}
			str_buffer[k++] = ch;
		}while(ch!='"'&&k<STR_SIZE);
		if(ch != '"'){
			error(lineno, 38);
			k = 0;
		}
		str_buffer[k-1] = '\0';//消去"
		symbol = STR_CONST_SYM;
		next_ch();
	}
	else if (ch == '\''){
		next_ch();
		num = (int) ch;
		symbol = CHAR_CONST_SYM;
		next_ch();
		if(ch != '\''){
			num = 0;
			error(lineno, 60);//少一个’
			// TODO:跳到正确的位置。
		}
		else
			next_ch();
	}
	else{
		symbol = ch_symbol(ch);		/* 当符号不满足上述条件时，全部按照单字符符号处理 */
		if(symbol == END_SYM){
			info(lineno, "不能识别的字符：");
			printf("%d\n", ch);
		}
		if (symbol != PERIOD_SYM)
			next_ch();
		
	}
	return 0;
}

