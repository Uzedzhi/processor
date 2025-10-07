#include <stdlib.h>

#include "../stack/stack.h"
#include "processor.h"
#include "../helpers/helpers.h"
#include "../usefullibs/error_manage.h"

// todo do smth about this pile of funcs
error_t do_push(proc_t *proc, stack_t *stack, size_t cur_line, stack_var_t value) {
    sassert(stack, ERR_PTR_NULL);

    stackPush(stack, value);
    return error;
}

error_t do_pop(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 1);
    
    stack_var_t a = 0;
    stackPop(stack, &a);
    return error;
}

error_t do_add(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a + b);
    return error;
}

error_t do_sub(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a - b);
    return error;
}

error_t do_div(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, b / a);
    return error;
}

error_t do_dump(proc_t *proc, size_t num_of_line) {
    sassert(proc, ERR_PTR_NULL);
    
    procDump(proc, num_of_line);
    return error;
}

error_t do_mul(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 2);

    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a * b);
    return error;
}

error_t do_out(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    stackPush(stack, a);
    printf("==============<%lf>==============\n", a);
    return error;
}

error_t do_popr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    proc->regs_values[num_of_reg] = a;
    return error;
}

error_t do_pushr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg) {
    sassert(stack, ERR_PTR_NULL);

    stackPush(stack, proc->regs_values[num_of_reg]);
    return error;
}

error_t do_qroot(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 3);
    
    stack_var_t a = 0;
    stack_var_t b = 0;
    stack_var_t c = 0;
    stackPop(stack, &c);
    stackPop(stack, &b);
    stackPop(stack, &a);
    double D = b * b - 4 * a * c;
    if (D > 0) {
        stackPush(stack, (-b + sqrt(D)) / (2 * a));
        stackPush(stack, (-b - sqrt(D)) / (2 * a));
    } else if (is_same(D, 0)) {
        stackPush(stack, -b / (2 * a));
    } else {
        stackPush(stack, 0);
    }
    return error;
}

void do_command(proc_t *proc, size_t num_of_line) {
    sassert(proc->stack, ERR_PTR_NULL);
    
    switch(proc->commands[num_of_line].num_of_command) {
        case PUSH_CMD:  do_push(proc, proc->stack, num_of_line, proc->commands[num_of_line].value);                            break;
        case POP_CMD:   do_pop(proc, proc->stack, num_of_line);                                                                break;
        case ADD_CMD:   do_add(proc, proc->stack, num_of_line);                                                                break;
        case SUB_CMD:   do_sub(proc, proc->stack, num_of_line);                                                                break;
        case DIV_CMD:   do_div(proc, proc->stack, num_of_line);                                                                break;
        case MUL_CMD:   do_mul(proc, proc->stack, num_of_line);                                                                break;
        case QROOT_CMD: do_qroot(proc, proc->stack, num_of_line);                                                              break;
        case PUSHR_CMD: do_pushr(proc, proc->stack, num_of_line, (int)proc->commands[num_of_line].value);   break;
        case POPR_CMD:  do_popr(proc, proc->stack, num_of_line, (int)proc->commands[num_of_line].value);    break;
        case OUT_CMD:   do_out(proc, proc->stack, num_of_line);                                                                break;
        case DUMP_CMD:  do_dump(proc, num_of_line);                                                         break;
        case HLT_CMD:   procDtor(&proc);                                                                break;
        case UNDEF_CMD: push_error(ERR_UNDEFINED_CMD);                                                      break;
        default:        push_error(ERR_UNDEFINED_CMD);                                                      break;
    }
}

error_t execute_file(proc_t *proc) {
    sassert(proc, ERR_PTR_NULL);

    size_t count = 0;
    while (count < proc->num_of_lines) {
        PROC_ERR_CHECK(proc, count, 0);

        if (proc->commands[count].num_of_command == HLT_CMD) {
            break;
        }
        do_command(proc, count);
        count++;
    }
    return error;
}

void print_commands(proc_t *proc, size_t num_of_line) {
    for (size_t i = 0; i < num_of_line; i++) {
        if (i % 10 == 0)
            putchar('\n');
        else
            printf("[%02X] ", proc->commands[i].num_of_command);
    }
    printf(RED "[%02X] " WHITE, proc->commands[num_of_line].num_of_command);
    for (size_t i = num_of_line + 1; i < proc->num_of_lines; i++) {
        if (i % 10 == 0)
            putchar('\n');
        else
            printf("[%02X] ", proc->commands[i].num_of_command);
    }
}

void print_regs(proc_t *proc, size_t num_of_line) {
    for (size_t i = 0; i < num_of_regs; i++) {
        printf("[%07.2lf] ", (double) proc->regs_values[i]);
    }
    putchar('\n');
    for (size_t i = 0; i < num_of_regs; i++) {
        printf("   %s    ", all_regs_str[i]);
    }
}

void procDump_internal(proc_t *proc, size_t num_of_line, const char * file_name, const char * func_name, size_t line) {
    int stack_errors = stackErrcheck(proc->stack, 0);
    stackDump(proc->stack, 0, stack_errors, file_name, func_name, line);

    printf(MAGENTA "\nprocessor info:\n" WHITE);
    printf("commands:");
    print_commands(proc, num_of_line);
    printf("\nreg info:\n");
    print_regs(proc, num_of_line);
    printf(RED "\n\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/ENDING DUMP\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n\n" WHITE);
}

void procDtor(proc_t **proc) {
    sassert(proc, ERR_PTR_NULL);
    sassert(*proc, ERR_PTR_NULL);

    stackDtor((*proc)->stack);
    free((*proc)->commands);
    free(*proc);
}

error_t create_proc(proc_t *proc, char * user_file_name, const char * file_name, const char * func_name, size_t line) {
    sassert(user_file_name, ERR_PTR_NULL);
    sassert(file_name,      ERR_PTR_NULL);
    sassert(func_name,      ERR_PTR_NULL);
    sassert(proc,           ERR_PTR_NULL);

    for (size_t i = 0; i < 8; i++) {
        proc->regs_values[i] = 0;
    }
    init_stack(stack, 50);
    proc->stack = stack;
    proc->commands = (line_format *) calloc(50, sizeof(line_format));
    proc->file_name = file_name;
    proc->func_name = func_name;
    proc->line = line;

    if (!is_file_exists(user_file_name)) {
        add_error(ERR_FILE_DOES_NOT_EXIST, user_file_name);
        return error;
    }
    FILE *fp = fopen(user_file_name, "r");

    size_t file_size = get_file_size(fp);
    fread(proc->commands, file_size, 1, fp);
    proc->num_of_lines = file_size / sizeof(line_format);

    fclose(fp);
    return error;
}

#define init_proc(proc, file_name) \
    proc_t *proc = (proc_t *) calloc(1, sizeof(proc_t));\
    create_proc(proc, file_name, __FILE__, __func__, __LINE__);

void print_help() {
    printf(MAGENTA "type 1) file which you need to execute\n" WHITE);
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        init_proc(proc, argv[1]);
        execute_file(proc);
        procDtor(&proc);
    }
    else 
        print_help();

    if (error.is_error == true) {
        print_error(error, error_text);
        return error.code;
    }
    printf(GREEN "file was successfully executed" WHITE);
    return 0;
}