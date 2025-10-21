#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../helpers/helpers.h"
#include "error_manage.h"



struct proc_t {
    stack_var_t regs_values[num_of_regs];
    unsigned long *procRAM;
    size_t procRAM_capacity;
    size_t procRAM_size;
    stack_t *return_stack;
    stack_t *stack;
    char debug_array[MAX_ARR_SIZE][MAX_STR_SIZE];
    line_format *commands;
    size_t num_of_lines;
    dimensions dims;
    header_t header;
    label *labels;
    const char * file_name;
    const char * func_name;
    size_t line;
};

#define GET_STR(x) #x


#if LEVEL_OF_CHECK > 1
#define CHECK_ERR_PROC(proc, stack, cur_line, pops_count) {\
    if (proc->commands[cur_line].num_of_command < 0 || proc->commands[cur_line].num_of_command >= num_of_commands) {\
        add_error(ERR_UNDEFINED_CMD, #cur_line);\
    }\
    if (get_size(stack) < pops_count || get_size(stack) > 0xfffffffff)\
        add_error(ERR_SIZE_INVALID);\
    if (error.is_error == true) {\
        procDump(proc, cur_line);\
        break;\
    }}
#else
#define CHECK_ERR_PROC(proc, stack, cur_line, pops_count) ;
#endif

#define CHECK_PROC_HEADER(proc) {\
    if ((proc->header).signature != SIGN)\
        add_error(ERR_INCORRECT_SIGN);\
    if ((proc->header).version != VERSION)\
        add_error(ERR_INCORRECT_VERSION);\
    if (error.is_error == true)\
        return error;\
}

#if LEVEL_OF_CHECK > 1
#define procDump(proc, cur_line) \
    procDump_internal(proc, cur_line, __FILE__, __func__, __LINE__);
#define place_debug_info(proc) \
    proc->file_name = file_name;\
    proc->func_name = func_name;\
    proc->line = line;
#else
#define procDump(proc, cur_line) ;
#define place_debug_info(proc) ;
#endif // PROC_ERR_CHECK

error_t do_push(proc_t *proc, stack_t *stack, size_t cur_line, stack_var_t value);
error_t do_pop(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_add(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_sub(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_div(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_qroot(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_mul(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_in(proc_t *proc, stack_t *stack, size_t num_of_line);
error_t do_dump(proc_t *stack, size_t num_of_line);
error_t do_pushr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg);
error_t do_popr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg);
error_t do_out(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_jb(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value, calcInst_t num_of_command);
error_t do_ret(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value);
error_t do_popm(proc_t *proc, stack_t *stack, size_t cur_line, size_t value);
error_t do_pushm(proc_t *proc, stack_t *stack, size_t cur_line, size_t value);
error_t do_draw(proc_t *proc, stack_t *stack, size_t num_of_line);
void do_short_skip(proc_t *proc, char *ch, size_t count);
void do_long_skip(proc_t *proc, char *ch, size_t count);
void skip_and_dump(proc_t *proc, char *ch, size_t count);
error_t execute_file(proc_t *proc, bool debug_flag);
error_t write_file_to_proc(proc_t *proc, char * user_file_name);
error_t init_proc_internal(proc_t *proc, const char * file_name, const char * func_name, size_t line);
void procDtor(proc_t **proc);
void print_help();
void print_regs(proc_t *proc, size_t num_of_line);
void print_commands(proc_t *proc, size_t num_of_line);
void procDump_internal(proc_t *proc, size_t num_of_line, const char * file_name, const char * func_name, size_t line);
bool do_command(proc_t *proc, size_t *num_of_line, calcInst_t num_of_command, stack_var_t value);
char * skip_spaces(char * str);

#endif // processor_h