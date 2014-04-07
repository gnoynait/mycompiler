#ifndef UTIL_H_INCLUDE
#define UTIL_H_INCLUDE
extern void clear_set(set_type s);
extern set_type statement_first;
extern set_type factor_first;
extern set_type const_first;
extern set_type block_first;

extern set_type new_set();
extern int in_set(enum symbol_type sym, set_type s);
extern void add_set(set_type s1, set_type s2);
extern void insert_set(set_type s, enum symbol_type sym);
/*打开初始化文件*/
extern void log_init(/*char * logfile_name*/);/*输出信息到文件上和屏幕上*/
extern void info(int line_num, char *info);
extern void error(int line_num, int error_id);
extern void fatal(int table_id);


#endif //UTIL_H_INCLUDE

