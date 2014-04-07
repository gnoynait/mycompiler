#include"global.h"
#include"assm.h"

FILE * assm_file;

#define BUFF_SIZE 200
//static char buffer[BUFF_SIZE];
//char tmp[BUFFER_SIZE];
static int block_idx;
static int block_level;
static int para_num;
static int block_size;
static int is_optim;


/****************************************/
static struct address register_pool[8];
static char * register_name[8] = {
	"EAX", "EBX", "ECX", "EDX", "EDI", "ESI", "EBP", "ESP"};

static struct code optim_code[CODE_TABLE_SIZE];
static int optim_code_end = 0;//包括
basic_block_start = 0;//包括
basic_block_end = 0;//不包括
int address_rank[100];
int next_block();
void rank_address(int start, int end);
void optim_translate(struct code ir_code);
enum register_type move_down_one();
void move_down(enum register_type reg);
/******************************************/

void translate(struct code ir_code);
void init_assm(FILE * code_file, int is_opt){
	assm_file = code_file;
	is_optim = is_opt;
	fprintf(assm_file, "\
.486\n\
.model flat, stdcall\n\
option casemap :none\n\
include     C:\\Masm32\\include\\windows.inc\n\
include     C:\\Masm32\\include\\kernel32.inc\n\
include     C:\\Masm32\\include\\msvcrt.inc\n\
include     C:\\masm32\\include\\masm32.inc\n\
include     C:\\Masm32\\macros\\macros.asm\n\
includelib  C:\\Masm32\\lib\\msvcrt.lib\n\
includelib  C:\\Masm32\\lib\\kernel32.lib\n\
includelib  C:\\masm32\\lib\\masm32.lib\n\
.code\n\
\n\
start:\n\
	push eax\n\
	call ____main\n\
	exit\n\n");
}
/**
*翻译中间代码
*从0到end,包括end
*/
void gen_assm(int end){
	int i;
	block_idx = get_current_block();
	block_level = get_current_level();
	para_num = identifier_value(block_idx);
	block_size = identifier_size(block_idx);
	if(!is_optim){
		
		for(i = 0; i <= end; i++){
			translate(get_ir_code(i));
		}
	}
	else{
		optim_code_end = end;
		basic_block_end = 0;
		basic_block_start = 0;
		while(next_block()){
			rank_address(basic_block_start, basic_block_end);
			for(i = basic_block_start; i < basic_block_end; i++){
				optim_translate(get_ir_code(i));
			}
		}
	}

}
//返回addr在汇编中的字符串
char * str_addr(struct address addr){
	int idx;
	int l;
	char tmp[BUFFER_SIZE];
	idx = addr.pointer;
	switch(addr.type){
	case VAR_ADDR:
		if(identifier_object(idx) == VARIABLE){
			l = identifier_level(idx);
			if(l == block_level){
				sprintf(tmp, "[ebp-%d]", identifier_offset(idx) * 4);
			}
			else{
				fprintf(assm_file,"\
	mov ebx, [ebp+%d]\n\
	lea ebx, [ebx-%d]\n", 8 + l * 4, 4 * identifier_offset(idx));
				sprintf(tmp, "[ebx]");
			}
		}
		else if(identifier_object(idx) == PROCEDURE || identifier_object(idx) == FUNCTION){
			sprintf(tmp, "[ebp+8]");
		}
		else if (identifier_object(idx) == VALUE_PARA)
			sprintf(tmp, "[ebp+%d]", 8 + 4*(para_num - identifier_offset(idx) + block_level));
		else if (identifier_object(idx) == VAR_PARA){
			fprintf(assm_file, "\
	mov ebx, [ebp+%d]\n", 8 + 4*(para_num - identifier_offset(idx) + block_level));
			sprintf(tmp, "[ebx]");
		}
		else if(identifier_object(idx) == CONSTANT)
			sprintf(tmp, "%d", identifier_value(idx));
		else if(identifier_object(idx) == ARRAY){
			sprintf(tmp, "[ebp-%d]", identifier_offset(idx) * 4);
		}
		else
			sprintf(tmp, "error");
		
		break;
		
	case VALUE_ADDR:
		sprintf(tmp, "%d", idx);
		break;
	case LABEL_ADDR:
		sprintf(tmp, "label%d:", idx);
		break;
	case STRING_ADDR:
		return get_str(idx);
	case ARRAY_ELEMENT_ADDR:
		fprintf(assm_file, "\
	lea ebx, %s\n\
	mov edx, %s\n\
	sal edx, 2\n\
	sub ebx, edx\n",str_addr(get_array_addr(idx)), str_addr(get_array_index_addr(idx)));
		sprintf(tmp, "[ebx]");
		break;
	case EMPTY_ADDR:
		sprintf(tmp, "empty");
	default:
		sprintf(tmp, "error");	
	}
	return get_str(store_str(tmp));
}

void translate(struct code ir_code){
	int i;
	switch(ir_code.instruction){
	case ADD_INS:	//	+	z=x+y
		fprintf(assm_file, "\
	mov eax, %s\n\
	add eax, %s\n\
	mov %s, eax\n",str_addr(ir_code.x),str_addr(ir_code.y),str_addr(ir_code.z));
		break;
	case MINUS_INS:	//	-	z=x-y
		fprintf(assm_file, "\
	mov eax, %s\n\
	sub eax, %s\n\
	mov %s, eax\n",str_addr(ir_code.x),str_addr(ir_code.y),str_addr(ir_code.z));
		break;
	case DIV_INS:	//	/	z=x/y
		fprintf(assm_file, "\
	mov eax, %s\n",str_addr(ir_code.x));
		fprintf(assm_file, "\
	mov ecx, %s\n\
	mov edx, 0\n\
	idiv ecx\n\
	mov %s, eax\n",str_addr(ir_code.y),str_addr(ir_code.z));
		break;
	case MULT_INS:	//	*	z=x*y
		fprintf(assm_file, "\
	mov eax, %s\n", str_addr(ir_code.x));
		fprintf(assm_file, "\
	mov edx, %s\n\
	imul edx\n\
	mov %s, eax\n",str_addr(ir_code.y),str_addr(ir_code.z));
		break;
	case NEG_INS:	// z = -x
		fprintf(assm_file, "\
	mov eax, %s\n\
	neg eax\n\
	mov %s, eax\n",str_addr(ir_code.x),str_addr(ir_code.z));
		break;
	case INC_INS: //	加1	z++
		fprintf(assm_file, "\
	mov eax, %s\n\
	inc eax\n\
	mov %s, eax\n",str_addr(ir_code.z),str_addr(ir_code.z));
		break;
	case DEC_INS:	//	减1	z--
		fprintf(assm_file, "\
	mov eax, %s\n\
	dec eax\n\
	mov %s, eax\n",str_addr(ir_code.z),str_addr(ir_code.z));
		break;
		break;
	//ARRAY_INS,	//	z=x[y]

	case ASSIGN_INS:	//	赋值z=x
		fprintf(assm_file, "\
	mov eax, %s\n\
	mov %s, eax\n",str_addr(ir_code.x),str_addr(ir_code.z));
		break;
	//CMP_INS,	//	比较	flag = z-0
	case LABEL_INS:	//	标签	lable z
		fprintf(assm_file, "label%d:\n", ir_code.z.pointer);
		break;
	case JMP_INS:	//	无条件跳转	goto z
		fprintf(assm_file, "\
	jmp label%d\n",ir_code.z.pointer);
		break;
	case JMPE_INS:	//	相等跳转	if x=y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	je label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPNE_INS:	//	不相等跳转	if x <> y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	jne label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPL_INS:	//	小于跳转		if x < y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	jl label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPNL_INS:	//	大于等于跳转 if x >= y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	jge label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPG_INS:	//	大于跳转		if x > y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	jg label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPNG_INS:	//	小于等于跳转	if x <= y goto z
		fprintf(assm_file, "\
	mov eax, %s\n\
	cmp eax, %s\n\
	jle label%d\n",str_addr(ir_code.x),str_addr(ir_code.y),ir_code.z.pointer);
		break;
	//IF_INS,		//	.IF
	//ELSE_INS,	//	.else
	//ENDIF_INS,	//	.endif
	
	case VAR_PARA_INS:	//变量参数		z
		fprintf(assm_file, "\
	lea eax, %s\n\
	push eax\n",str_addr(ir_code.z));
		break;//do nothing.
	case VALUE_PARA_INS:	//值参数		z
		fprintf(assm_file, "\
	mov eax, %s\n\
	push eax\n",str_addr(ir_code.z));
		break;
	case CALL_INS:	//	调用过程函数	z 过程地址
		if(block_level == identifier_level(ir_code.x.pointer)){//过程函数的level比其内容level小1
			fprintf(assm_file, "\
	push ebp\n");
			for(i = identifier_level(ir_code.x.pointer) - 1; i > 0 ; i--){
				fprintf(assm_file, "\
	mov ebx, [ebp+%d]\n\
	push ebx\n", 8+i*4);
			}
		}
		else
			for(i = identifier_level(ir_code.x.pointer); i > 0 ; i--){
				fprintf(assm_file, "\
	lea ebx, [ebp+%d]\n\
	push ebx\n", 8+i*4);
			}
		fprintf(assm_file, "\
	push eax\n\
	call __%s\n\
	add esp, %d\n\
	mov %s, eax\n",identifier_name(ir_code.x.pointer),(1+identifier_value(ir_code.x.pointer)+identifier_level(ir_code.x.pointer))*4/*消去返回值，display区，参数*/, str_addr(ir_code.z));
		break;
	case RET_INS:	//	过程函数返回	z
		fprintf(assm_file, "\
	mov eax, [ebp+8]\n\
	pop edi\n\
	pop esi\n\
	pop ebx\n\
	add esp, %d\n\
	pop ebp\n\
	ret\n\
__%s endp\n\n",block_size * 4,identifier_name(block_idx));
		break;
	case READC_INS:	//	读入字符		char->z
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%c\"), addr %s\n", str_addr(ir_code.z));	
		break;
	/*case READL_INS:	//	读入换行字符
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%c\"), addr 0\n");
		break;*/
	case READI_INS://	读入整数		int->z
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%d\"), addr %s\n", str_addr(ir_code.z));
		break;
	case WRITEC_INS:	//	写字符			out << z
		fprintf(assm_file, "\
	invoke crt_printf, SADD(\"%%c \"), dword ptr %s\n", str_addr(ir_code.z));
		break;
	case WRITEI_INS:	//	写整数			out << z
		fprintf(assm_file, "\
	invoke crt_printf, SADD(\"%%d \"), dword ptr %s\n", str_addr(ir_code.z));
		break;
	case WRITES_INS:	//	写字符串		out	<< z
		fprintf(assm_file, "\
	print chr$(\"%s\")\n", str_addr(ir_code.z));
		break;
	case WRITEL_INS:	//	写字符串		out	<< z
		fprintf(assm_file, "\
	print chr$(10)\n", str_addr(ir_code.z));
		break;
	case BLOCK_INS://进入子程序
		fprintf(assm_file, "\n\
__%s proc\n\
	push ebp\n\
	mov ebp, esp\n\
	sub esp, %d\n\
	push ebx\n\
	push esi\n\
	push edi\n", identifier_name(block_idx), block_size * 4);//block_size是内部空间大小
		break;
	case ERROR_INS:		//无操作，出错
		break;
	default:
		break;
		}
	//ADDR_INS,		//取地址 取 x[y]的地址存到z
	//STORE_INS		//将x 的内容存到地址z
}

void assm_over(){
	fprintf(assm_file,"\
end start\n");
}
/**********************************************/
/**********************************************/



int is_block_end(int i){
	if(i >= optim_code_end)
		return 1;
	switch(optim_code[i].instruction){
	case LABEL_INS:
	case JMP_INS:
	case JMPE_INS:
	case JMPNE_INS:
	case JMPL_INS:	
	case JMPNL_INS:	
	case JMPG_INS:
	case JMPNG_INS:	
	case RET_INS:
		return 1;
	default:
		return 0;
	}
}
int next_block(){
	if(basic_block_end > optim_code_end)
		return 0;
	basic_block_start = basic_block_end;
	while(!is_block_end(basic_block_end))
		basic_block_end++;
	basic_block_end++;
	return 1;
}

void rank_address(int start, int end){
	int i;
	struct address index;
	for(i = 0; i < 100; i++){
		address_rank[i] = 0;
	}
	for(i = start; i < end ; i++){
		if(optim_code[i].x.type == VAR_ADDR){
			address_rank[optim_code[i].x.pointer] ++;
		}
		else if(optim_code[i].x.type == ARRAY_ELEMENT_ADDR){
			index = get_array_index_addr(optim_code[i].x.pointer);
			if(index.type == VAR_ADDR)
				address_rank[index.pointer]++;
		}
		if(optim_code[i].y.type == VAR_ADDR){
			address_rank[optim_code[i].y.pointer] ++;
		}
		else if(optim_code[i].y.type == ARRAY_ELEMENT_ADDR){
			index = get_array_index_addr(optim_code[i].y.pointer);
			if(index.type == VAR_ADDR)
				address_rank[index.pointer]++;
		}
		if(optim_code[i].z.type == VAR_ADDR){
			address_rank[optim_code[i].z.pointer] ++;
		}
		else if(optim_code[i].z.type == ARRAY_ELEMENT_ADDR){
			index = get_array_index_addr(optim_code[i].z.pointer);
			if(index.type == VAR_ADDR)
				address_rank[index.pointer]++;
		}
	}
}
			

allocation_register(){
	
}
void load_optim_code(int end){
	int i;
	for(i = 0; i <= end; i++){
		optim_code[i] = get_ir_code(i);
	}
	optim_code_end = end;
}
char * optim_addr(struct address addr){
	int idx;
	int l;
	int i;
	enum register_type reg;
	char tmp[BUFFER_SIZE];
	for(i = 0; i < 8; i++){
		if(register_pool[i].type == addr.type && 
			register_pool[i].pointer == addr.pointer &&
			addr.type != ARRAY_ELEMENT_ADDR)
		{
			return register_name[i];
		}
	}

	idx = addr.pointer;
	switch(addr.type){
	case VAR_ADDR:
		if(identifier_object(idx) == VARIABLE){
			l = identifier_level(idx);
			if(l == block_level){
				sprintf(tmp, "[ebp-%d]", identifier_offset(idx) * 4);
			}
			else{
				reg = move_down_one();
				fprintf(assm_file,"\
	mov %s, [ebp+%d]\n\
	lea %s, [%s-%d]\n",register_name[reg],  8 + l * 4, register_name[reg], register_name[reg],4 * identifier_offset(idx));
				sprintf(tmp, "[%s]", register_name[reg]);
			}
		}
		else if(identifier_object(idx) == PROCEDURE || identifier_object(idx) == FUNCTION){
			sprintf(tmp, "[ebp+8]");
		}
		else if (identifier_object(idx) == VALUE_PARA)
			sprintf(tmp, "[ebp+%d]", 8 + 4*(para_num - identifier_offset(idx) + block_level));
		else if (identifier_object(idx) == VAR_PARA){
			reg = move_down_one();
			fprintf(assm_file, "\
	mov %s, [ebp+%d]\n",register_name[reg],  8 + 4*(para_num - identifier_offset(idx) + block_level));
			sprintf(tmp, "[%s]", register_name[reg]);
		}
		else if(identifier_object(idx) == CONSTANT)
			sprintf(tmp, "%d", identifier_value(idx));
		else if(identifier_object(idx) == ARRAY){
			sprintf(tmp, "[ebp-%d]", identifier_offset(idx) * 4);
		}
		else
			sprintf(tmp, "error");
		
		break;
		
	case VALUE_ADDR:
		sprintf(tmp, "%d", idx);
		break;
	case LABEL_ADDR:
		sprintf(tmp, "label%d:", idx);
		break;
	case STRING_ADDR:
		return get_str(idx);
	case ARRAY_ELEMENT_ADDR:
		move_down(EBX);
		move_down(EDX);
		fprintf(assm_file, "\
	lea ebx, %s\n\
	mov edx, %s\n\
	sal edx, 2\n\
	sub ebx, edx\n",optim_addr(get_array_addr(idx)), optim_addr(get_array_index_addr(idx)));
		sprintf(tmp, "[ebx]");
		break;
	case EMPTY_ADDR:
		sprintf(tmp, "empty");
	default:
		sprintf(tmp, "error");	
	}
	return get_str(store_str(tmp));
}



void move_to_memory(enum register_type reg){
	if(register_pool[reg].type == EMPTY_ADDR)
		return;
	fprintf(assm_file, "\
	mov %s, %s\n", str_addr(register_pool[reg]), register_name[reg]);
	register_pool[reg].type = EMPTY_ADDR;
}
void move_down(enum register_type reg){
	if(register_pool[reg].type == EMPTY_ADDR)
		return;
	fprintf(assm_file, "\
	mov %s, %s\n", optim_addr(register_pool[reg]), register_name[(int) reg]);
	register_pool[reg].type = EMPTY_ADDR;
}
enum register_type move_down_one(){
	int i; 
	int min = 1000;
	enum register_type r = EAX;
	for(i = EAX; i <= EDX; i++){
		if(address_rank[register_pool[i].pointer] < min)
			r = i;
	}
	move_down(r);
	return r;
}
void move_to(enum register_type reg, struct address addr){
	if(register_pool[reg].type = EMPTY_ADDR){
		fprintf(assm_file, "\
	mov %s, %s\n",register_name[(int) reg],optim_addr(addr));
		register_pool[reg] = addr;
	}
	else if(register_pool[reg].type == addr.type && 
		register_pool[reg].pointer == addr.pointer)
	{
		return;
	}
	else{
		fprintf(assm_file, "\
	mov %s, %s\n\
	mov %s, %s\n",str_addr(register_pool[reg]), register_name[reg], register_name[(int) reg],optim_addr(addr));
		register_pool[reg] = addr;
	}
}
enum register_type load_on(struct address addr, enum register_type except){
	int i;
	int min = 10000;
	enum register_type r = EAX;
	for(i = EAX; i <EDX; i++){
		if(register_pool[i].type == addr.type && register_pool[i].pointer == addr.pointer
			|| register_pool[i].type == EMPTY_ADDR && i != except)
			return i;
	}
	for(i = EAX; i <= EDX; i++){
		if(address_rank[register_pool[i].pointer] < min && i != except)
			r = i;
	}
	move_down(r);
	fprintf(assm_file, "\
	mov %s, %s\n", register_name[r], optim_addr(addr));
	register_pool[r] = addr;
	return r;
}


void optim_translate(struct code ir_code){
	int i;
	int reg;
	switch(ir_code.instruction){
	case ADD_INS:	//	+	z=x+y
		reg = load_on(ir_code.x,NO_REG);
		fprintf(assm_file, "\
	add eax, %s\n",optim_addr(ir_code.y));
		register_pool[reg] = ir_code.z;
		break;
	case MINUS_INS:	//	-	z=x-y
		reg = load_on(ir_code.x,NO_REG);
		fprintf(assm_file, "\
	sub eax, %s\n",optim_addr(ir_code.y));
		register_pool[reg] = ir_code.z;
		break;
	case DIV_INS:	//	/	z=x/y
		move_down(EDX);
		move_to(EAX, ir_code.x);
		move_to(ECX, ir_code.y);
		fprintf(assm_file, "\
	mov edx, 0\n\
	idiv ecx\n");
		register_pool[ECX] = ir_code.z;
		break;
	case MULT_INS:	//	*	z=x*y
		move_to(EAX, ir_code.x);
		move_to(EDX, ir_code.y);
		fprintf(assm_file, "\
	imul edx\n");
		register_pool[EAX] = ir_code.z;
		break;
	case NEG_INS:	// z = -x
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	neg %s\n",register_name[reg]);
		register_pool[reg] = ir_code.z;
		break;
	case INC_INS: //	加1	z++
		reg = load_on(ir_code.z, NO_REG);
		fprintf(assm_file, "\
	inc %s\n", register_name[reg]);
		register_pool[reg] = ir_code.z;
		break;
	case DEC_INS:	//	减1	z--
		reg = load_on(ir_code.z, NO_REG);
		fprintf(assm_file, "\
	dec %s\n", register_name[reg]);
		register_pool[reg] = ir_code.z;
		break;
	//ARRAY_INS,	//	z=x[y]

	case ASSIGN_INS:	//	赋值z=x
		reg = load_on(ir_code.x, NO_REG);
		register_pool[reg] = ir_code.z;
		break;
	//CMP_INS,	//	比较	flag = z-0
	case LABEL_INS:	//	标签	lable z
		fprintf(assm_file, "label%d:\n", ir_code.z.pointer);
		break;
	case JMP_INS:	//	无条件跳转	goto z
		fprintf(assm_file, "\
	jmp label%d\n",ir_code.z.pointer);
		break;
	case JMPE_INS:	//	相等跳转	if x=y goto z
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	je label%d\n",register_name[reg], optim_addr(ir_code.y), ir_code.z.pointer);
		break;
	case JMPNE_INS:	//	不相等跳转	if x <> y goto z
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	jne label%d\n",register_name[reg], optim_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPL_INS:	//	小于跳转		if x < y goto z
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	jl label%d\n",register_name[reg], optim_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPNL_INS:	//	大于等于跳转 if x >= y goto z
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	jge label%d\n",register_name[reg], optim_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPG_INS:	//	大于跳转		if x > y goto z
		reg = load_on( ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	jg label%d\n",register_name[reg], optim_addr(ir_code.y),ir_code.z.pointer);
		break;
	case JMPNG_INS:	//	小于等于跳转	if x <= y goto z
		reg = load_on(ir_code.x, NO_REG);
		fprintf(assm_file, "\
	cmp %s, %s\n\
	jle label%d\n",register_name[reg], optim_addr(ir_code.y),ir_code.z.pointer);
		break;
	
	case VAR_PARA_INS:	//变量参数		z
		reg = move_down_one(EAX);
		fprintf(assm_file, "\
	lea %s, %s\n\
	push %s\n", register_name[reg]);
		break;//do nothing.
	case VALUE_PARA_INS:	//值参数		z
		reg = load_on(ir_code.z, NO_REG);
		fprintf(assm_file, "\
	push %s\n",register_name[reg]);
		break;
	case CALL_INS:	//	调用过程函数	z 过程地址
		if(block_level == identifier_level(ir_code.x.pointer)){//过程函数的level比其内容level小1
			fprintf(assm_file, "\
	push ebp\n");
			for(i = identifier_level(ir_code.x.pointer) - 1; i > 0 ; i--){
				fprintf(assm_file, "\
	mov ebx, [ebp+%d]\n\
	push ebx\n", 8+i*4);
			}
		}
		else
			for(i = identifier_level(ir_code.x.pointer); i > 0 ; i--){
				fprintf(assm_file, "\
	lea ebx, [ebp+%d]\n\
	push ebx\n", 8+i*4);
			}
		fprintf(assm_file, "\
	push eax\n\
	call __%s\n\
	add esp, %d\n\
	mov %s, eax\n",identifier_name(ir_code.x.pointer),(1+identifier_value(ir_code.x.pointer)+identifier_level(ir_code.x.pointer))*4/*消去返回值，display区，参数*/, str_addr(ir_code.z));
		break;
	case RET_INS:	//	过程函数返回	z
		fprintf(assm_file, "\
	mov eax, [ebp+8]\n\
	popa\n\
	add esp, %d\n\
	pop ebp\n\
	ret\n\
__%s endp\n\n",block_size * 4,identifier_name(block_idx));
		break;
	case READC_INS:	//	读入字符		char->z
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%c\"), addr %s\n", optim_addr(ir_code.z));	
		break;
	/*case READL_INS:	//	读入换行字符
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%c\"), addr 0\n");
		break;*/
	case READI_INS://	读入整数		int->z
		fprintf(assm_file, "\
	invoke crt_scanf, SADD(\"%%d\"), addr %s\n", optim_addr(ir_code.z));
		break;
	case WRITEC_INS:	//	写字符			out << z
		fprintf(assm_file, "\
	invoke crt_printf, SADD(\"%%c \"), dword ptr %s\n", optim_addr(ir_code.z));
		break;
	case WRITEI_INS:	//	写整数			out << z
		fprintf(assm_file, "\
	invoke crt_printf, SADD(\"%%d \"), dword ptr %s\n", optim_addr(ir_code.z));
		break;
	case WRITES_INS:	//	写字符串		out	<< z
		fprintf(assm_file, "\
	print chr$(\"%s\")\n", optim_addr(ir_code.z));
		break;
	case WRITEL_INS:	//	写字符串		out	<< z
		fprintf(assm_file, "\
	print chr$(10)\n", optim_addr(ir_code.z));
		break;
	case BLOCK_INS://进入子程序
		fprintf(assm_file, "\n\
__%s proc\n\
	push ebp\n\
	mov ebp, esp\n\
	sub esp, %d\n\
	pusha\n", identifier_name(block_idx), block_size * 4);//block_size是内部空间大小
		break;
	case ERROR_INS:		//无操作，出错
		break;
	default:
		break;
	}

}