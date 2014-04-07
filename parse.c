#include"global.h"
#include"parse.h"
#include"scan.h"
#include"util.h"
#include"table.h"

int function_index;//�����ķ��ű��±꣬���Ϊ0�� ��ʾ���Ǻ���,�����һ���ӳ����к���������ֵ������Ϊ0
//�����м����Ŀյ�ַ
const struct address empty_address = { EMPTY_ADDR, 0};

void skip_to(set_type s){
	while(!in_set(symbol, s))
		next_sym();
}
/*void test_symbol(set_type s1, set_type s2, char * message){
	if(!in_set(symbol,  s1)){
		info(lineno, message);
		add_set(s2, s1);
		next_sym();
		while(!in_set(symbol, s2)){
			next_sym();
		}		
	}
	free(s1);
	free(s2);
}*/

//�����������ɹ��򷵻���ֵ�����򷵻�0
int parse_const(){
	set_type s;
	int sign = 1;
	s = new_set();
	add_set(s, const_first);
	if(!in_set(symbol, s)){
		info(lineno, "�����������");
		add_set(s, factor_first);
		add_set(s, block_first);
		skip_to(s);
	}
	if(symbol == CHAR_CONST_SYM)
		return num;
	if(symbol == PLUS_SYM){
		next_sym();
	}
	else if(symbol == MINUS_SYM){
		sign = -1;
		next_sym();
	}
	if(symbol == INTEGER_CONST_SYM){
		return sign * num;
	}
	else{
		info(lineno, "�����������");
		skip_to(s);
	}	
	free(s);
	return 0;
}
static enum type_type get_type(struct address a){
	switch(a.type){
	case VAR_ADDR:
		return identifier_type(a.pointer);
	case VALUE_ADDR:
		return INT_TYPE;
	case ARRAY_ELEMENT_ADDR:
		return get_type(get_array_addr(a.pointer));
	default:
		info(lineno, "����ƥ�����");
		return INT_TYPE;
	}
}
//�Ƚ������Ƿ������ͬ�����ͣ����ؽ�����ͣ���ͬ�򱨴�
enum type_type match_type(struct address a, struct address b){
	enum type_type at, bt;
	at = get_type(a);
	bt = get_type(b);
	if(at != bt){
		info(lineno, "���Ͳ�һ��");
		return INT_TYPE;
	}
	return at;
}

struct address parse_array_element(){
	int ident_idx;
	set_type s;
	enum type_type type;
	struct address array_index;
	struct address element;
	struct address array_name;

	ident_idx = find_identifier(identifier);
	if(ident_idx == 0){
		info(lineno, "����δ����");
	}
	else if(identifier_object(ident_idx) != ARRAY){
		info(lineno, "��ʶ����������");
	}
	type = identifier_type(ident_idx);
	array_name.pointer = ident_idx;
	array_name.type = VAR_ADDR;

	next_sym();
	if(symbol != LSQUARE_SYM){
		info(lineno, "��������ȱ��[");
		s = new_set();
		add_set(s, factor_first);
		insert_set(s, RSQUARE_SYM);
		add_set(s, statement_first);
		add_set(s, const_first);
		skip_to(s);
		free(s);
	}
	else next_sym();
	
	array_index = parse_expression();

	if(symbol != RSQUARE_SYM){
		info(lineno, "��������ȱ��]");
	}
	else 
		next_sym();
	return array_element(ident_idx, array_index);
}


/*	�������̺�������ʱʱʵ�ʲ�����	*/
void parse_real_paralist(int function_idx){
	int para_count = 0;
	int pidx;
	struct address paddr;
	int para_num = identifier_value(function_idx);
	while(para_count < para_num && symbol == IDENTIFIER_SYM){
		para_count++;
		
		pidx = find_identifier(identifier);
		if(pidx == 0){
			info(lineno, "����δ����");
		}
		/*if(identifier_object(function_idx + para_count) == VAR_PARA){
			if(identifier_object(pidx) != VARIABLE){
				info(lineno, "����������������ֵ���ܴ���var ����");
			}
			
		}*/

		paddr = parse_expression();
		/*if((identifier_object(function_idx + para_count) == VAR_PARA) &&
			!((paddr.pointer == pidx) && (paddr.type == VAR_ADDR))){
				if(identifier_object(pidx) != ARRAY)
					info(lineno, "�������ݴ��󣺷Ǳ������ܴ���VAR����");
		}
		else */if(identifier_type(pidx) != get_type(paddr))
			info(lineno, "��������ƥ�����");
		if(identifier_object(function_idx + para_count) == VAR_PARA){
			gen_code(VAR_PARA_INS, empty_address, empty_address, paddr);
		}else
			gen_code(VALUE_PARA_INS, empty_address, empty_address, paddr);

		if(symbol == SEMICOLON_SYM){
			info(lineno, "����Ӧ��\'��\'�ָ�");
			next_sym();
		}
		else if(symbol == COMMA_SYM)
			next_sym();
		else break;
	}

	if(para_count != para_num)
		info(lineno, "������Ŀ����");
	//next_sym();
}

struct address parse_call_statement(){
	struct address x,z;
	int i;
	i = find_identifier(identifier);
	if(i == 0){
		info(lineno, "��������δ����");
	}
	/*if(identifier_object(i) == PROCEDURE){
		info(lineno, "���̲�����Ϊ����");
	}
	else 
	if(identifier_object(i) != FUNCTION){
		info(lineno, "�������̵��ó���");
	}*/
	if(identifier_object(i) != FUNCTION && identifier_object(i) != PROCEDURE){
		info(lineno,"�������̵��ó���,��ʶ�����Ǻ����͹���");
	}
	x.type = VAR_ADDR;
	x.pointer = i;
	z = new_temp(identifier_type(i));
	next_sym();
	if(symbol != LROUND_SYM){
		info(lineno, "��������ȱ��\'(\'");
	}
	else 
		next_sym();
	parse_real_paralist(i);
	if(symbol != RROUND_SYM){
		info(lineno, "��������ȱ��\')\'");
	}
	else
		next_sym();
	gen_code(CALL_INS, x, empty_address, z);
	return z;
}



//��������
struct address parse_factor(){
	int ident_idx; 
	set_type skip_end;
	struct address addr = {VALUE_ADDR, 0};//����ʱ����ֵ0
	if(!in_set(symbol, factor_first)){
		skip_end = new_set();
		add_set(skip_end, factor_first);
		add_set(skip_end, statement_first);
		skip_to(skip_end);
		free(skip_end);
	}
	while(in_set(symbol, factor_first)){
		if(symbol == INTEGER_CONST_SYM){
			addr.type = VALUE_ADDR;
			addr.pointer = num;
			next_sym();
		}
		else if(symbol== IDENTIFIER_SYM){
			ident_idx = find_identifier(identifier);
			if(ident_idx == 0){
				info(lineno, "����δ����");
			}
			switch(identifier_object(ident_idx)){
			case CONSTANT:
				addr.pointer = identifier_value(ident_idx);
				addr.type = VALUE_ADDR;
				next_sym();
				break;
			case VARIABLE:
			case VAR_PARA:
			case VALUE_PARA:
				addr.pointer = ident_idx;
				addr.type = VAR_ADDR;
				next_sym();
				break;
			case ARRAY:
				addr = parse_array_element();
				break;
			case FUNCTION:
				addr = parse_call_statement();
				break;
			default://PROCEDURE
				info(lineno, "�����в����й��̵���");
				addr = parse_call_statement();
			}
		}

		else if(symbol == LROUND_SYM){
			next_sym();
			addr = parse_expression();
			if(symbol != RROUND_SYM)
				info(lineno, "����ƥ�����");
			else
				next_sym();
		}

		if(in_set(symbol, factor_first))
			info(lineno, "�﷨����,��������");
	}
	return addr;
}

struct address parse_term(){
	struct address x, y, z;
	x = parse_factor();
	while(symbol == MULT_SYM || symbol == DIV_SYM){
		if(symbol == MULT_SYM){
			next_sym();
			y = parse_factor();
			z = new_temp(match_type(x, y));
			gen_code(MULT_INS, x, y, z);
			x = z;
		}
		else if(symbol == DIV_SYM){
			next_sym();
			y = parse_factor();
			z = new_temp(match_type(x, y));
			gen_code(DIV_INS, x, y, z);
			x = z;
		}
	}
	return x;
}

/* ���ڽ������ʽ */
struct address parse_expression()
{
	struct address x, y, z;
	enum instruction_type ins;
	if(symbol == PLUS_SYM){
		next_sym();
		x = parse_term();
	}
	else if(symbol == MINUS_SYM){
		next_sym();
		x = parse_term();
		z = new_temp(get_type(x));
		gen_code(NEG_INS, x, empty_address, z);
		x = z;
	}
	else{
		x = parse_term();
	}

	while(symbol == MINUS_SYM || symbol == PLUS_SYM){
		if(symbol == MINUS_SYM)
			ins = MINUS_INS;
		else
			ins = ADD_INS;
		next_sym();
		y = parse_term();
		z = new_temp(match_type(x, y));
		gen_code(ins, x, y, z);
		/*if(symbol == MINUS_SYM)
			gen_code(MINUS_INS, x, empty_address, z);
		else
			gen_code(ADD_INS, x, empty_address, z);*/
		x = z;
	}
	return x;
}

/*void parse_array_assign_statement(){
	int ident_idx;
	struct address index_addr, temp_addr;
	struct address array_addr, element_addr, source_addr;
	ident_idx = find_identifier(identifier);
	array_addr.type = VAR_ADDR;
	array_addr.pointer = ident_idx;
	if(identifier_object(ident_idx) != ARRAY)
		info(lineno, "��������");
	else
		next_sym();
	if(symbol != LSQUARE_SYM)
		info(lineno, "��������ȱ��\'[\'");
	else
		next_sym();
	index_addr = parse_expression();
	if(symbol != RSQUARE_SYM)
		info(lineno, "��������ȱ��\']\'");
	else
		next_sym();
	if(symbol != BECOMES_SYM){
		if(symbol == EQL_SYM)
			info(lineno, "��ֵ���Ӧʹ��\':=\'");
		else
			next_sym();
	}
	else
		next_sym();
	source_addr = parse_expression();
	temp_addr = new_temp(NO_TYPE);
	gen_code(ADDR_INS, array_addr, index_addr, temp_addr);
	gen_code(STORE_INS, source_addr, empty_address, temp_addr);
}
*/

//����ֵ���
//dest_idx�Ǳ���ֵ���ķ��ű�����
int parse_assign_statement( )
{
	struct address source, dest;
	int ident_idx;
	enum object_type type = VARIABLE;
	ident_idx = find_identifier(identifier);
	if(ident_idx == 0){
		info(lineno, "��ʶ��δ����");
	}
	else if(identifier_object(ident_idx) == ARRAY){
		dest = parse_array_element();
	}
	else{
		dest.pointer = ident_idx;
		dest.type = VAR_ADDR;
		next_sym();
	}	
	if(symbol != BECOMES_SYM){
		info(lineno, "��ֵ���ȱ��:=");
		if(symbol == EQL_SYM)
			next_sym();
	}
	next_sym();
	source = parse_expression();
	gen_code(ASSIGN_INS, source, empty_address, dest);
	return ident_idx;
}

void parse_read_statement()
{
	int id;
	struct address v;
	enum type_type type;
	if(symbol != READ_SYM)
		info(lineno, "read���Ӧ��read��ͷ");
	else
		next_sym();
	if(symbol != LROUND_SYM)
		info(lineno, "readȱ��\'��\'");
	else
		next_sym();
	do{
		if(symbol == COMMA_SYM)
			next_sym();
		if(symbol != IDENTIFIER_SYM){
			info(lineno, "read������ӦΪ����");
			continue;
		}

		id = find_identifier(identifier);
		if(id == 0)
			info(lineno, "����δ����");
		v.type = VAR_ADDR;
		v.pointer = id;
		if((identifier_object(id) != VARIABLE) && 
			(identifier_object(id)  != VAR_PARA) &&
			(identifier_object(id) != VALUE_PARA) &&
			(identifier_object(id) != FUNCTION))
			info(lineno, "�����Ӧ��Ϊ����");
		type = get_type(v);
		if(type == CHAR_TYPE)
			gen_code(READC_INS,empty_address, empty_address, v);
		else
			gen_code(READI_INS, empty_address, empty_address, v);
		next_sym();
	}while(symbol == COMMA_SYM);

	if(symbol != RROUND_SYM)
		info(lineno, "�����Ӧ��)");
	else
		next_sym();
}

void parse_write_statement()
{
	int str_id;
	struct address a, b;
	if(symbol != WRITE_SYM)
		info(lineno, "write���ȱ��write");
	else
		next_sym();
	if(symbol != LROUND_SYM){
		info(lineno, "ȱ��\'(\'");
	}
	else
		next_sym();
	if(symbol == RROUND_SYM)
		info(lineno, "write�����û������");
	else if(symbol == STR_CONST_SYM){
		str_id = store_str(str_buffer);
		a.type = STRING_ADDR;
		a.pointer = str_id;
		gen_code(WRITES_INS, empty_address, empty_address, a);
		
		next_sym();
		if(symbol == COMMA_SYM){
			b = parse_expression();
			if(get_type(b) == CHAR_TYPE)
				gen_code(WRITEC_INS, empty_address, empty_address, b);
			else
				gen_code(WRITEI_INS, empty_address, empty_address, b);
		}

	}
	else{
		a = parse_expression();
		if(get_type(a) == CHAR_TYPE)
			gen_code(WRITEC_INS, empty_address, empty_address, a);
		else
			gen_code(WRITEI_INS, empty_address, empty_address, a);
	}
	gen_code(WRITEL_INS, empty_address, empty_address, empty_address);
	if(symbol != RROUND_SYM){
		info(lineno, "д���Ӧ��\')\'����");
	}
	else
		next_sym();
}
	


void parse_for_statement(){
	int id;
	set_type s;
	struct address l_start, l_end;
	struct address addr = empty_address, start,limit;
	enum instruction_type ins = ERROR_INS;
	enum type_type type;
	l_start = new_label();
	l_end = new_label();
	if(symbol != FOR_SYM){
		info(lineno, "for���Ӧ��for��ͷ");
	}
	else
		next_sym();
	if(symbol != IDENTIFIER_SYM){
		if(symbol == LROUND_SYM){
			info(lineno, "for����в�Ӧ����\'(\'");
			next_sym();
		}
		/*else{
			info(lineno, "for�����ѭ������Ӧ���Ǳ���");
			s = new_set();
			add_set(s, statement_first);
			insert_set(s, TO_SYM);
			insert_set(s, DOWNTO_SYM);
			skip_to(s);
			free(s);
		}*/
	}
	else{
		id = find_identifier(identifier);
		if(id == 0)
			info(lineno, "����δ����");
		addr.type = VAR_ADDR;
		addr.pointer = id;
		type = identifier_type(id);
		next_sym();
	}
	if(symbol != BECOMES_SYM){
		if(symbol == EQL_SYM){
			info(lineno, "for�����Ӧ����\':=\'������\'=\'");
			next_sym();
		}
		else
			info(lineno,"for�����ȱ��\':=\'");
	}
	else
		next_sym();
	start = parse_expression();
	gen_code(ASSIGN_INS, start, empty_address, addr);

	if(symbol == TO_SYM){
		ins = JMPG_INS;
		next_sym();
	}
	else if(symbol == DOWNTO_SYM){
		ins = JMPL_INS;
		next_sym();
	}
	else
		info(lineno,"ȱ��TO��DOWNTO");
	
	limit = parse_expression();
	gen_code(LABEL_INS, empty_address, empty_address, l_start);
	match_type(addr, limit);
	gen_code(ins, addr, limit, l_end);
	if(symbol != DO_SYM)
		info(lineno, "for���ȱ��do");
	else
		next_sym();
	parse_statement();
	if(ins == JMPG_INS)
		gen_code(INC_INS,empty_address, empty_address, addr);
	else
		gen_code(DEC_INS, empty_address, empty_address, addr);

	parse_statement();	
	gen_code(JMP_INS, empty_address, empty_address, l_start);
	gen_code(LABEL_INS, empty_address, empty_address, l_end);
}


void parse_case_statement(){
	struct address a, b;
	struct address l_end;
	struct address l_next, l_current;
	set_type s;
	l_end = new_label();
	l_next = new_label();
	//l_next = l_end;//��һ��ֵû����
	if(symbol != CASE_SYM)
		info(lineno, "������Ӧ��case��ͷ");
	else
		next_sym();
	a = parse_expression();
	if(symbol != OF_SYM)
		info(lineno, "������ȱ��of");
	//else
		//next_sym();
	/*while(symbol != END_SYM){		
		gen_code(LABEL_INS, empty_address, empty_address, l_next);
		l_next = new_label();
		do{
			if(symbol == CHAR_CONST_SYM){
				if(get_type(a) == INT_TYPE)
					info(lineno, "���Ͳ�ƥ��");
				b.type = VALUE_ADDR;
				b.pointer = num;
				gen_code(JMPNE_INS,a, b, l_next);
				next_sym();
			}
			else if(symbol == INTEGER_CONST_SYM){
				if(get_type(a) == CHAR_TYPE)
					info(lineno, "���Ͳ�ƥ��");
				b.type = VALUE_ADDR;
				b.pointer = num;
				gen_code(JMPNE_INS,a, b, l_next);
				next_sym();
			}

			//TODO BUG
			else{
				info(lineno, "case�����ֻ���ǳ���");
				s = new_set();
				add_set(s, const_first);
				add_set(s, statement_first);
				add_set(s, statement_first);
				add_set(s, factor_first);
				insert_set(s, COMMA_SYM);
				insert_set(s, COLON_SYM);
				skip_to(s);
				free(s);
			}
		}while(symbol == COMMA_SYM);
		if(symbol != COLON_SYM)
			info(lineno, "case�����ȱ��\':\'");
		else 
			next_sym();
		parse_statement();
		if(symbol == SEMICOLON_SYM)
			next_sym();
		else{
			if(symbol == END_SYM)
				break;
			else
				info(lineno, "ȱ��\';\'");
			s = new_set();
			insert_set(s, SEMICOLON_SYM);
			insert_set(s, COMMA_SYM);
			add_set(s, const_first);
			skip_to(s);
			if(symbol == SEMICOLON_SYM || symbol == COMMA_SYM){
				next_sym();
			}	
		}
		gen_code(JMP_INS, empty_address, empty_address, l_end);
	}
	gen_code(LABEL_INS, empty_address, empty_address, l_end);
	if(symbol != END_SYM)
		info(lineno, "case��䵱��end����");
	else
		next_sym();
		*/
	do{
		next_sym();
		l_current = new_label();
		gen_code(LABEL_INS, empty_address, empty_address, l_next);
		do{
			if(symbol == COMMA_SYM)
				next_sym();
			if(symbol == CHAR_CONST_SYM){
				if(get_type(a) == INT_TYPE)
					info(lineno, "���Ͳ�ƥ��");
				b.type = VALUE_ADDR;
				b.pointer = num;
				gen_code(JMPE_INS,a, b, l_current);
				next_sym();
			}
			else if(symbol == INTEGER_CONST_SYM){
				if(get_type(a) == CHAR_TYPE)
					info(lineno, "���Ͳ�ƥ��");
				b.type = VALUE_ADDR;
				b.pointer = num;
				gen_code(JMPE_INS,a, b, l_current);
				next_sym();
			}

			//TODO BUG
			else{
				info(lineno, "case�����ֻ���ǳ���");
				next_sym();
				/*s = new_set();
				add_set(s, const_first);
				add_set(s, statement_first);
				add_set(s, statement_first);
				add_set(s, factor_first);
				insert_set(s, COMMA_SYM);
				insert_set(s, COLON_SYM);
				skip_to(s);
				free(s);*/
			}
		}while(symbol == COMMA_SYM);
		if(symbol != COLON_SYM){
			info(lineno, "for�����ȱ��\':\'");
		}
		else
			next_sym();
		l_next = new_label();
		gen_code(JMP_INS, empty_address, empty_address, l_next);
		gen_code(LABEL_INS, empty_address, empty_address, l_current);
		//l_current = l_next;
		parse_statement();
		gen_code(JMP_INS, empty_address, empty_address, l_end);


	}while(symbol == SEMICOLON_SYM);
	gen_code(LABEL_INS, empty_address, empty_address, l_next);
	gen_code(LABEL_INS, empty_address, empty_address, l_end);
	if(symbol != END_SYM){
		info(lineno, "for���ȱ��end");
	}
	else
		next_sym();
}


void parse_if_statement()
{
	struct address x, y;
	enum instruction_type ins = JMPNE_INS;
	struct address l1, l2;

	
	l1 = new_label();
	l2 = new_label();
	

	if(symbol != IF_SYM){
		info(lineno, "if���Ӧif��ʼ");
	}
	else
		next_sym();
	x = parse_expression();
	if(symbol == LSS_SYM)
		ins = JMPNL_INS;
	else if (symbol == LEQ_SYM)
		ins = JMPG_INS;
	else if ( symbol == EQL_SYM)
		ins = JMPNE_INS;
	else if (symbol == GTR_SYM)
		ins = JMPNG_INS;
	else if(symbol == GEQ_SYM)
		ins = JMPL_INS;
	else
		info(lineno, "��ϵ���ʽ��û�й�ϵ��");
	//next_sym();
	y = parse_expression();
	gen_code(ins, x, y, l1);

	if(symbol != THEN_SYM){
		info(lineno, "if�����ȱ��then");
	}
	else
		next_sym();
	//TODO:�����NO_TYPE�Ƿ���
	parse_statement();
	gen_code(JMP_INS, empty_address, empty_address, l2);
	gen_code(LABEL_INS, empty_address, empty_address, l1);
	if(symbol == ELSE_SYM){
		//gen_code(JMP_INS, empty_address, empty_address, l2);
		next_sym();
		parse_statement();
		
		//next_sym();
	}	
	gen_code(LABEL_INS, empty_address, empty_address, l2);
}
void parse_statement(){
	int ident_idx;
	if(symbol == IF_SYM)
		parse_if_statement();
	else if(symbol == CASE_SYM)
		parse_case_statement();
	else if(symbol == BEGIN_SYM)
		parse_multi_statement();
	else if(symbol == READ_SYM)
		parse_read_statement();
	else if(symbol == WRITE_SYM)
		parse_write_statement();
	else if(symbol == FOR_SYM)
		parse_for_statement();
	else if(symbol == IDENTIFIER_SYM){
		ident_idx = find_identifier(identifier);
		if(ident_idx == 0)
			info(lineno, "��ʶ��δ����");
		else
			switch(identifier_object(ident_idx)){
			case VARIABLE:
			
			case VALUE_PARA:
			case VAR_PARA:
			case FUNCTION:
			case ARRAY:
				parse_assign_statement();
				break;
			case PROCEDURE:			
				parse_call_statement();
				break;
			default:          //CONSTANT
				info(lineno, "�������ܸ�ֵ");
				skip_to(statement_first);
		}
		
	}
}


void parse_const_dec(){
	int value = 0;
	set_type s;
	if(symbol != IDENTIFIER_SYM){
		info(lineno, "������������ӦΪ��ʶ��");
		s = new_set();
		add_set(s, block_first);
		insert_set(s, IDENTIFIER_SYM);
		skip_to(s);
		free(s);
	}
	if(symbol == IDENTIFIER_SYM){
		if(!local_registed(identifier))//��ͬһ���в���
			register_identifier(identifier);
		else
			info(lineno, "�ظ�����");
		next_sym();		
		if (symbol == EQL_SYM || symbol == BECOMES_SYM){
			if(symbol == BECOMES_SYM)
				info(lineno,"��������Ӧʹ��'='");
			next_sym();
			value = parse_const();
			if(symbol == INTEGER_CONST_SYM)
				fix_identifier(1,INT_TYPE,CONSTANT,0, value);
			else
				fix_identifier(1,CHAR_TYPE,CONSTANT,0, value);
			next_sym();
		}
		else{
			info(lineno, "�������岻������ȱ��\'=\'");
			//if(in_set(symbol, const_first))
		}
		

		if(symbol == COMMA_SYM){
			next_sym();
			parse_const_dec();
		}
		else if(symbol == SEMICOLON_SYM){
			next_sym();
			return;
		}
		else{
			s = new_set();
			add_set(s, block_first);
			skip_to(s);
			free(s);
		}
	}
}


//������������
void parse_var_dec(){
	int count = 0;	//һ�������ĸ���
	int size = 0;	//�����С
	int ix;
	enum type_type type = INT_TYPE;
	enum object_type object = VARIABLE;	//������������
	set_type s;
	if(symbol != IDENTIFIER_SYM)
		info(lineno, "������������");
	while(symbol == IDENTIFIER_SYM){
		/*ix = find_identifier(identifier);
		if(ix != 0)
			info(lineno, "�����ظ�����");*/
		if(local_registed(identifier))
			info(lineno, "�����ظ�����");
		register_identifier(identifier);
		count ++;
		next_sym();
		if(symbol == COLON_SYM){
			next_sym();
			break;
		}
		if(symbol != COMMA_SYM){
			info(lineno, "����֮��Ӧ�ԣ��ָ�");
			if(symbol == IDENTIFIER_SYM)
				continue;
		}
		next_sym();
	}

	//��������
	if(symbol == INTEGER_SYM){
		next_sym();
		type = INT_TYPE;
	}
	else if(symbol == CHAR_SYM){
		type = CHAR_TYPE;
		next_sym();
	}
	else if(symbol == ARRAY_SYM){
		next_sym();
		object = ARRAY;

		if(symbol != LSQUARE_SYM)
			info(lineno, "�����ȱ��[");
		next_sym();
		if(symbol == INTEGER_CONST_SYM){
			if(num <= 0){
				info(lineno, "�����Сֻ��Ϊ������");
				size = 0;
			}
			size = num;
		}
		next_sym();
		if(symbol != RSQUARE_SYM){
			info(lineno, "��������ȱ��]");
		}
		next_sym();
		if(symbol != OF_SYM){
			info(lineno, "��������ȱ��of");
		}
		next_sym();
		if(symbol == INTEGER_SYM){
			type = INT_TYPE;
		}
		else if(symbol == CHAR_SYM){
			type = CHAR_TYPE;
		}
		else{
			info(lineno, "�����������ʹ���");
		}
		next_sym();
		if(symbol != SEMICOLON_SYM){
			info(lineno, "��������ȱ��;");
		}
	}
	fix_identifier(count, type, object,size, 0);
	if(symbol != SEMICOLON_SYM){
		s = new_set();
		add_set(s, block_first);
		insert_set(s, IDENTIFIER_SYM);
		skip_to(s);
		free(s);
		info(lineno, "��������Ӧ��\';\'��β");
	}
	else
		next_sym();

	if(symbol == IDENTIFIER_SYM){
		parse_var_dec();
	}
}

/* �����������̵��õĲ����� */
void parse_paralist(){
	enum object_type para_object = VALUE_PARA;
	enum type_type para_type = INT_TYPE;
	int count = 0;
	if(symbol == VAR_SYM){
		next_sym();
		para_object = VAR_PARA;
	}
	if(symbol != IDENTIFIER_SYM)
		info(lineno, "��ʽ����������");
	if(symbol == RROUND_SYM)
		info(lineno, "��ʽ��������Ϊ��");
	while(symbol == IDENTIFIER_SYM){
		count ++;
		if(local_registed(identifier))
			info(lineno, "�������ظ�����");
		register_identifier(identifier);
		next_sym();

		if(symbol == COLON_SYM){
			next_sym();
			break;
		}

		if(symbol != COMMA_SYM){
			info(lineno, "������������\',\'");
			if(symbol == IDENTIFIER_SYM)
				continue;
		}
		next_sym();
	}
	if(symbol == CHAR_SYM)
		para_type = CHAR_TYPE;
	else if (symbol == INTEGER_SYM)
		para_type = INT_TYPE;
	else
		info(lineno, "���������ʹ���");
	fix_identifier(count, para_type, para_object, 0, 0);
	next_sym();
	if(symbol == SEMICOLON_SYM){
		next_sym();
		parse_paralist();
	}
}
	


//�������̺�������������ǹ����򷵻�NO_TYPE
//����Ǻ����򷵻�INT_TYPE��CHAR_TYPE
enum type_type parse_proc_func_dec(){
	enum object_type object = FUNCTION;
	enum type_type func_type = NO_TYPE;
	if(symbol == PROC_SYM)
		object = PROCEDURE;
	next_sym();
	if(symbol != IDENTIFIER_SYM)
		info(lineno, "���������˵���к���������");
	else{
		if(local_registed(identifier)){
			info(lineno, "���������������ظ�");
		}
		register_block(identifier, object);
		next_sym();
	}
	if(symbol != LROUND_SYM)
		info(lineno, "ȱ��(");
	else
		next_sym();
	if(symbol == VAR_SYM || symbol == IDENTIFIER_SYM)
	{
		parse_paralist();
	}
	if(symbol != RROUND_SYM)
		info(lineno, "���̺���˵��ȱ��)");
	else
		next_sym();
	if(object == FUNCTION){
		if(symbol != COLON_SYM)
			info(lineno, "��������ȱ�٣�");
		else
			next_sym();
		if(symbol == CHAR_SYM)
			func_type = CHAR_TYPE;
		else if(symbol == INTEGER_SYM)
			func_type = INT_TYPE;
		else
			info(lineno, "����������������");
		next_sym();
	}
	if(symbol != SEMICOLON_SYM){
		info(lineno, "��������˵��ȱ�٣�");
		skip_to(block_first);
	}
	else
		next_sym();
	fix_block(func_type);
	return func_type;
}

//�����������
void parse_multi_statement(){
	set_type s1;
	if(symbol != BEGIN_SYM){
		info(lineno, "�������ȱ��begin");
		s1 = new_set();
		add_set(s1, statement_first);
		skip_to(s1);
		free(s1);
	}
	else 
		next_sym();
	parse_statement();
	//TODO��FIXME! �������������������һ�䵱û��';'
	while(symbol == SEMICOLON_SYM){
		next_sym();
		parse_statement();
	}
	if(symbol != END_SYM){
		info(lineno, "�������Ӧ��end��β");
		s1 = new_set();
		add_set(s1, block_first);
		add_set(s1, statement_first);
		skip_to(s1);
		free(s1);
	}
	else
		next_sym();
}
	
//�����ֳ���
//��const,var��begin��ʼ�����������̺�������������)
//block_type�ǳ�������
void parse_block(enum type_type block_type){
	enum type_type sub_block_type;
	struct address block_addr;
	set_type set;
	
	set = new_set();
	add_set(set, block_first);
	if(!in_set(symbol, set)){
		info(lineno, "Ӧ��const,var,�� begin");
		skip_to(set);
	}
	if(symbol == CONST_SYM){
		next_sym();
		parse_const_dec();
	}
	else skip_to(block_first);
	if(symbol == VAR_SYM){
		next_sym();
		parse_var_dec();
	}
	while(symbol == FUNC_SYM || symbol == PROC_SYM){
		sub_block_type = parse_proc_func_dec();
		if(symbol != CONST_SYM && symbol != VAR_SYM && symbol != FUNC_SYM && symbol != PROC_SYM && symbol != BEGIN_SYM){
			info(lineno, "�ӳ�����������");
		}
		parse_block(sub_block_type);
	}

	if(symbol != BEGIN_SYM){
		info(lineno, "�������Ӧ��begin��ʼ");
		skip_to(statement_first);
	}
	//���code_table()
	clear_code_table();
	block_addr.pointer = get_current_block();
	block_addr.type = VAR_ADDR;

	gen_code(BLOCK_INS, empty_address, empty_address, block_addr);

	parse_multi_statement();

	fix_block(block_type);
	gen_code(RET_INS, empty_address, empty_address, empty_address);
#ifdef DEBUG
	list_symbol_table();
	list_code();
#endif
	list_symbol_table();
	list_code();
	gen_assm(get_cur_code_idx());
	pop_block();
	if(get_current_level() != 0){
		if(symbol != SEMICOLON_SYM)
			info(lineno, "�������̶���ȱ��;");
		else
			next_sym();
	}
	else{
		if(symbol != PERIOD_SYM)
		info(lineno, "����Ӧ��.����");
	}
	
}

//�﷨��������ʼ��
void parse_programe(){
	next_sym();
	parse_block(NO_TYPE);
	if(symbol != PERIOD_SYM){
		info(lineno, "ȱ��\'.\'");
	}
	assm_over();
}

