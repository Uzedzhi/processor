#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../helpers/helpers.h"
#include "../usefullibs/error_manage.h"

struct proc_t {
    stack_var_t regs_values[8];
    stack_t *stack;
    line_format *commands;
    const char * file_name;
    const char * func_name;
    size_t line;
    size_t num_of_lines;
};
// todo level_of_check
#define PROC_ERR_CHECK(proc, cur_line, pops_count) {\
    if (proc->commands[cur_line].num_of_command < 0 || proc->commands[cur_line].num_of_command >= num_of_commands) {\
        add_error(ERR_UNDEFINED_CMD, #cur_line);\
    }\
    if (get_size(proc->stack) < pops_count || get_size(proc->stack) > 0xfffffffff)\
        add_error(ERR_SIZE_INVALID);\
    if (error.is_error == true) {\
        procDump(proc, cur_line);\
        return error;\
    }}

// todo level_of_check
#define procDump(proc, cur_line) \
    procDump_internal(proc, cur_line, __FILE__, __func__, __LINE__);


error_t do_push(proc_t *proc, stack_t *stack, size_t cur_line, stack_var_t value);
error_t do_pop(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_add(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_sub(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_div(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_qroot(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_mul(proc_t *proc, stack_t *stack, size_t cur_line);
error_t do_dump(proc_t *stack, size_t num_of_line);
error_t do_pushr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg);
error_t do_popr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg);
error_t do_out(proc_t *proc, stack_t *stack, size_t cur_line);
void procDtor(proc_t **proc);
void print_help();
void procDump_internal(proc_t *proc, size_t num_of_line, const char * file_name, const char * func_name, size_t line);
void do_command(proc_t *proc, size_t num_of_line);

#endif // processor_h