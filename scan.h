#ifndef SCAN_H_INCLUDED
#define SCAN_H_INCLUDED
#include"global.h"

extern int open_sourcefile(char * filename);
extern char identifier[ID_SIZE];    /* 当前ident, 多出的一个字节用于存放0 */
//extern char ch;
extern enum symbol_type symbol;
extern int num;
extern char str_buffer[BUFFER_SIZE];
int next_sym();

#endif // SCAN_H_INCLUDED
