#include"global.h"
#include"util.h"

#define ERR_MAX 65
#define MSG_MAX 8
char * err[ERR_MAX];
char * msg[MSG_MAX];
FILE * log_file;
//��Ҫ��ʼ��
set_type statement_first;
set_type factor_first;
set_type const_first;
set_type block_first;

void init_set(set_type s){
	int i;
	for(i = 0; i < SYMBOL_NUM; i++)
		s[i] = 0;
}
void clear_set(set_type s){
	init_set(s);
}

set_type new_set()
{
	set_type s;
	s = (set_type) malloc(sizeof(int) * SYMBOL_NUM);
	init_set(s);
	return s;
}

int in_set(enum symbol_type sym, set_type s){
	return s[(int) sym];
}

void add_set(set_type s1, set_type s2){
	int i;
	for(i = 0; i < SYMBOL_NUM; i++){
		s1[i] = s1[i] + s2[i];
	}
}
void insert_set(set_type s, enum symbol_type sym)
{
	s[(int) sym] = 1;
}

void util_init(/*char * logfile_name*/){

	statement_first = new_set();
	statement_first[IDENTIFIER_SYM] = 1;
	statement_first[BEGIN_SYM] = 1;
	statement_first[IF_SYM] = 1;
	statement_first[CASE_SYM] = 1;
	statement_first[READ_SYM] = 1;
	statement_first[WRITE_SYM] = 1;
	statement_first[FOR_SYM] = 1;

	block_first = new_set();
	block_first[VAR_SYM] = 1;
	block_first[CONST_SYM] = 1;
	block_first[FUNC_SYM] = 1;
	block_first[PROC_SYM] = 1;
	block_first[BEGIN_SYM] = 1;
	block_first[PERIOD_SYM] = 1;

	factor_first = new_set();
	factor_first[INTEGER_CONST_SYM] = 1;
	factor_first[IDENTIFIER_SYM] = 1;
	factor_first[LROUND_SYM] = 1;

	const_first = new_set();
	const_first[PLUS_SYM] = 1;
	const_first[MINUS_SYM] = 1;
	const_first[INTEGER_CONST_SYM] = 1;
	const_first[CHAR_CONST_SYM] = 1;


	/*log_file = fopen(logfile_name, "w");
	if(log_file == NULL){
		printf("CAN NOT OPEN LOG FILE!\n");;
		printf("PROGRAM WILL EXIT.\n");
		getchar();
	}
	*/
	msg[1]="identfier";
	msg[2]="prcedures";
	msg[3]="temp";
	msg[4]="address";
	msg[5]="levels";
	msg[6]="code";
	msg[7]="strings";

	//num=0;
	err[0]="undef id|�ñ�ʶ��δ����";
	err[1]="multi def|��ʶ���ظ�����";
	err[2]="identifier|Ӧ�Ǳ�ʶ��";
	err[3]="program incomplete|�������ش��󣬳����޷���������";
	err[4]=")Ӧ��'��'";
	err[5]=":|Ӧ��'��'����˵������ʱ�����д�ð��";
	err[6]="syntax|�Ƿ����ţ�������������÷��źͺ����ĳЩ����";
	err[7]="ident,var|��ʽ�������У��β�˵��Ӧ�Ա�ʶ����var��ͷ";
	err[8]="of";
	err[9]="Ӧ��'��'";
	err[10]="id,array|���Ͷ�������Ա�ʶ��,array��record��ͷ";
	err[11]="[|Ӧ��[";
	err[12]="]|Ӧ��]";
	err[13]=":|Ӧ��':'";
	err[14]=";|Ӧ��'��'";
	err[15]="fun.type|�������������integer��real��char����";
	err[16]="=|Ӧ��'='��'��='��ֻ���ڸ�ֵ�����ʹ�ã���������˵����ʹ��";
	err[17]="boolean";
	err[18]="convar typ|��for�����ѭ������ֻ����interger��char��";
	err[19]="type|for����г�ֵ����ֵ���ʽ������ѭ������������ͬ";
	err[20]="prog.param";
	err[21]="too big|��̫��";
	err[22]=".|�����β��'��'��������Ӧ��begin��end��";
	err[23]="type(case)";
	err[24]="character|�Ƿ��ַ�";
	err[25]="const id|�ڳ��������У��Ⱥź�������ǳ���������ʶ��";
	err[26]="index type|�±���ʽ����������˵���е��±�������ͬ";
	err[27]="index bound|����˵����,�½粻�ܳ����Ͻ�,ͬʱ���ǵ����ͱ�����ͬ(ͬΪ����,���ַ�),ʵ�����½��ǷǷ���";
	err[28]="no array|û������������";
	err[29]="type id|Ӧ�����ͱ�ʶ��";
	err[30]="undef type|������û�ж���(������ݹ鶨������";
	err[31]="no record|û�������ļ�¼";
	err[32]="bool type";
	err[33]="arith type|���������ʽ�����Ͳ��Ϸ�";
	err[34]="integer|mod����Ĳ���������������";
	err[35]="types|��ȽϵĶ�������ͱ�����ͬ����������һ�������ͣ���һ����ʵ��";
	err[36]="param type|ʵ�κͶ�Ӧ���βα�����ͬ������Ҫô�β�Ϊʵ����ֵ��ʵ��Ϊ���ͻ��ַ��ͣ�Ҫô�β�Ϊ���͵�ֵ��ʵ��Ϊ�ַ���";err[37]="variable id|Ӧ�Ǳ���";
	err[38]="string|�ַ���̫����ȱ��һ���ַ�";
	err[39]="no. of pars|ʵ�θ������βθ�������";
	err[40]="real number|С�����û������";
	err[41]="type|read��write�Ĳ������Ͳ���ȷ";
	err[42]="real type|�ñ��ʽӦΪʵ��";
	err[43]="integer";
	err[44]="var,const|���ʽ�в��ܳ������ͻ���̱�ʶ��";
	err[45]="var,proc|Ӧ�Ǳ�������̡�������ʶ��";
	err[46]="types(:=)|�ڸ�ֵ����б�������Ӧ����ʽ������ͬ������ǰ��Ϊʵ�Ͷ�����Ϊ����";  err[47]="typ(case)";
	err[48]="type";
	err[49]="store ovfl|�ڴ����";
	err[50]="constant|Ӧ�ǳ���";
	err[51]=":=|Ӧ��'��='";
	err[52]="then|Ӧ��then ";
	err[53]="while|Ӧ��while";
	err[54]="do|Ӧ��do";
	err[55]="to downto|Ӧ��to��downto";
	err[56]="begin|Ӧ��begin";
	err[57]="end|Ӧ��end";
	err[58]="factor|���ӱ����Ա�ʶ������������'��'��ʼ";
	err[59]="�ַ����г����쳣�ַ�";
	err[60]="�ַ��쳣��ȱ��'";

}

void info(int line_num, char *info){
	//fprintf(log_file, info);
	printf("LINE %d:%s\n", line_num, info);
}

void error(int line_num, int error_id){
	//fprintf(log_file, "%s\n", err[error_id]);
	printf("%s\n", err[error_id]);
}
void fatal(int table_id){
	char* msg[8];
	error(lineno, 49);
	//fprintf(log_file, "compiler table for %s is too small!\n", msg[table_id]);
	printf("compiler table for %s is too small!\n", msg[table_id]);
	exit(0);
}


/********************************************************/
/*														*/
/*						Symbol Set						*/
/*														*/
/********************************************************/

int test_symbol_set[SYMBOL_NUM];
int * init_symbol_set(){
	int i;
	for(i = 0; i < SYMBOL_NUM; i++)
		test_symbol_set[i] = 0;
	return test_symbol_set;
}