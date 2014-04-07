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

void translate(struct code ir_code);
void init_assm(FILE * code_file){
	assm_file = code_file;
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
	for(i = 0; i <= end; i++){
		translate(get_ir_code(i));
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
		break;
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