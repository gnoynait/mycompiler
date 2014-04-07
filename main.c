#include"global.h"
#include"scan.h"
#include"table.h"
#include"parse.h"
#include"assm.h"

FILE * source_file;  //source file
FILE * code_file;    //output code file
FILE * log_file; //log file
int lineno = 0; //源程序的行号
int main(){
	log_file = fopen("log.txt", "w");
	if(log_file == NULL){
		printf("CAN NOT OPEN LOG FILE!\n");
		getchar();
		return -1;
	}

	source_file = fopen("source.txt", "r");
	if (source_file == NULL){
		printf("CAN NOT OPEN SOURCE FILE!\n");
		printf("PROGRAM WILL EXIT.\n");
		getchar();
		return -1;
	}

	code_file = fopen("output.asm", "w");
	if(code_file == NULL){
		printf("can't open code file!\n");
		getchar();
		return -1;
	}
	init_assm(code_file);
	log_init();
	parse_programe();
	printf("编译结束.\n\n");
	
	fclose(source_file);
	fclose(code_file);
	fclose(log_file);
	getchar();
	return 0;
}
