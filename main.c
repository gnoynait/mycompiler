#include"global.h"
#include"scan.h"
#include"table.h"
#include"parse.h"
#include"assm.h"
#include<string.h>

FILE * source_file;  //source file
FILE * code_file;    //output code file
//FILE * log_file; //log file

int lineno = 0; //源程序的行号

int main(int argc, char * argv[]){
	int i;
	char source_file_name[STR_SIZE];
	int is_optim = 0;	//优化
	/*log_file = fopen("log.txt", "w");
	if(log_file == NULL){
		printf("CAN NOT OPEN LOG FILE!\n");
		getchar();
		return -1;
	}*/
	strcpy(source_file_name, "source.txt");
	if(argc > 3)
		argc = 3;
	for(i = 1; i < argc; i++){
		if(!strcmp(argv[i], "-o"))
			is_optim = 1;
		else
			strcpy(source_file_name, argv[i]);
	}
	source_file = fopen(source_file_name, "r");
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

	init_assm(code_file, 0);
	util_init();

	parse_programe();
	printf("\n\n编译结束.\n");
	
	fclose(source_file);
	fclose(code_file);
	free(const_first);
	free(factor_first);
	free(statement_first);
	free(block_first);
	//fclose(log_file);
	getchar();
	return 0;
}
