#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "../helpers/helpers.h"
#include "error_manage.h"

extern error_t error;
struct proc_t
{
    stack_var_t regs_values[num_of_regs];
    RAM procRAM;
    stack_t *return_stack;
    stack_t *stack;
    line_format *commands;
    size_t num_of_lines;
    VID video;
    header_t header;
    label *labels;
#if LEVEL_OF_CHECK > 1
    const char *file_name;
    const char *func_name;
    size_t line;
#endif
};

#define GET_STR(x) #x

#define CHECK_PROC_HEADER(proc)                       \
    {                                                 \
        if ((proc->header).signature != SIGN)         \
            add_error(ERR_INCORRECT_SIGN, "none");    \
        if ((proc->header).version != VERSION)        \
            add_error(ERR_INCORRECT_VERSION, "none"); \
        if (error.is_error == true)                   \
            return error;                             \
    }

#if LEVEL_OF_CHECK > 1
#define procDump(proc, cur_line) \
    procDump_internal(proc, cur_line, __FILE__, __func__, __LINE__);
#define place_debug_info(proc)   \
    proc->file_name = file_name; \
    proc->func_name = func_name; \
    proc->line = line;
#else
#define procDump(proc, cur_line) ;
#define place_debug_info(proc) ;
#endif // PROC_ERR_CHECK

#if LEVEL_OF_CHECK > 1
#define CHECK_ERR_PROC(proc, stack, cur_line, pops_count)                                                              \
    {                                                                                                                  \
        if (proc->commands[cur_line].num_of_command < 0 || proc->commands[cur_line].num_of_command >= num_of_commands) \
        {                                                                                                              \
            add_error(ERR_UNDEFINED_CMD, #cur_line);                                                                   \
        }                                                                                                              \
        if (get_size(stack) < pops_count || get_size(stack) > 0xfffffffff)                                             \
            add_error(ERR_SIZE_INVALID);                                                                               \
        if (error.is_error == true)                                                                                    \
        {                                                                                                              \
            procDump(proc, cur_line);                                                                                  \
            break;                                                                                                     \
        }                                                                                                              \
    }
#else
#define CHECK_ERR_PROC(proc, stack, cur_line, pops_count) ;
#endif

void print_commands(proc_t *proc, size_t num_of_line);
void procDump_internal(proc_t *proc, size_t num_of_line, const char *file_name, const char *func_name, size_t line);
void print_memory_info(proc_t *proc, size_t num_of_line);
void print_next_command(size_t count, proc_t *proc, size_t num_of_line);
void print_regs(proc_t *proc, size_t num_of_line);
void do_short_skip(proc_t *proc, char *ch, size_t count);
void do_long_skip(proc_t *proc, char *ch, size_t count);
void skip_and_dump(proc_t *proc, char *ch, size_t count);
error_t execute_file(proc_t *proc, bool debug_flag);
error_t write_file_to_proc(proc_t *proc, char *user_file_name);
error_t init_proc_internal(proc_t *proc, const char *file_name, const char *func_name, size_t line);
void procDtor(proc_t **proc);
char *skip_spaces(char *str);

#endif // processor_h