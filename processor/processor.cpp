#include <stdlib.h>

#include "../stack/stack.h"
#include "processor.h"
#include "../helpers/helpers.h"
#include "error_manage.h"
#include <time.h>

error_t do_push(proc_t *proc, stack_t *stack, size_t cur_line, stack_var_t value) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 0);

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

error_t do_in(proc_t *proc, stack_t *stack, size_t num_of_line) {
    sassert(proc, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, num_of_line, 0);
 
    stack_var_t a = 0;
    printf("type your number: ");
    scanf("%lf", &a);
    stackPush(stack, a);
    return error;
}

error_t do_pow(proc_t *proc, stack_t *stack, size_t num_of_line, int value) {
    sassert(proc, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, num_of_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    a = (stack_var_t)pow(a, value);
    stackPush(stack, a);
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
    printf("==============<%lf>==============\n", a);
    return error;
}

error_t do_jmp(proc_t *proc, stack_t *stack, size_t *cur_line, int value) {
    sassert(proc, ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, *cur_line, 0);
    *cur_line = value - 1;
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

error_t do_jb(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value, calcInst_t num_of_command) {
    sassert(proc, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, *cur_line, 2);

    stack_var_t a = 0;
    stack_var_t b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    switch(num_of_command) {
        case JA:  if (a < b)            *cur_line = value - 1; break;
        case JB:  if (a > b)            *cur_line = value - 1; break;
        case JE:  if (is_same(a, b))    *cur_line = value - 1; break;
        case JAE: if (a <= b)           *cur_line = value - 1; break;
        case JBE: if (a >= b)           *cur_line = value - 1; break;
        case JNE: if (is_same(a, b))    *cur_line = value - 1; break;
    }
    return error;
}

error_t do_pushr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg) {
    sassert(stack, ERR_PTR_NULL);
    PROC_ERR_CHECK(proc, cur_line, 0);

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

void do_command(proc_t *proc, size_t *num_of_line, calcInst_t num_of_command, stack_var_t value) {
    sassert(proc->stack, ERR_PTR_NULL);
    sassert(num_of_line, ERR_PTR_NULL);
    switch(num_of_command) {
        case PUSH:  do_push(proc, proc->stack, *num_of_line, value);                    break;
        case POP:   do_pop(proc, proc->stack, *num_of_line);                            break;
        case ADD:   do_add(proc, proc->stack, *num_of_line);                            break;
        case SUB:   do_sub(proc, proc->stack, *num_of_line);                            break;
        case DIV:   do_div(proc, proc->stack, *num_of_line);                            break;
        case MUL:   do_mul(proc, proc->stack, *num_of_line);                            break;
        case POW:   do_pow(proc, proc->stack, *num_of_line, value);                     break;
        case QROOT: do_qroot(proc, proc->stack, *num_of_line);                          break;
        case JMP:   do_jmp(proc, proc->stack, num_of_line, (int) value);                break;
        case PUSHR: do_pushr(proc, proc->stack, *num_of_line, (int) value);             break;
        case POPR:  do_popr(proc, proc->stack, *num_of_line, (int) value);              break;
        case JE:
        case JA:
        case JAE:
        case JBE:
        case JNE:
        case JB:    do_jb(proc, proc->stack, num_of_line, (int) value, num_of_command); break; 
        case OUT:   do_out(proc, proc->stack, *num_of_line);                            break;
        case IN:    do_in(proc, proc->stack, *num_of_line);                             break;
        case DUMP:  do_dump(proc, *num_of_line);                                        break;
        case HLT:   procDtor(&proc);                                                    break;
        case UNDEF_CMD: add_error(ERR_UNDEFINED_CMD, all_commands_text[num_of_command]);         break;
        default:        push_error(FATAL_ERROR, "command code undefined");                  break;
    }
}

#define PROC_HEADER_CHECK(header) {\
    if (header.signature != SIGN)\
        add_error(ERR_INCORRECT_SIGN);\
    if (header.version != VERSION)\
        add_error(ERR_INCORRECT_VERSION);\
    if (error.is_error == true)\
        return error;\
    }
        

error_t execute_file(proc_t *proc, bool debug_flag) {
    sassert(proc, ERR_PTR_NULL);

    clock_t start = clock();
    size_t count = 0;
    PROC_HEADER_CHECK(proc->header);
    
    while (count < proc->num_of_lines && proc->commands[count].num_of_command != HLT) {
        PROC_ERR_CHECK(proc, count, 0);

        if (debug_flag == true) {
            scanf("%*c");
            procDump(proc, count);
        }
        do_command(proc, &count, proc->commands[count].num_of_command, proc->commands[count].value);
        count++;
    }
    printf("time taken(in seconds): %lf\n", (double) (clock() - start) / CLOCKS_PER_SEC);
    return error;
}

void print_commands(proc_t *proc, size_t num_of_line) {
    for (size_t i = 0; i < proc->num_of_lines; i++) {
        if (i % 10 == 0)
            putchar('\n');
        if (i == num_of_line)
            printf(RED "[%02X] " WHITE, proc->commands[num_of_line].num_of_command);
        else
            printf("[%02X] ", (unsigned int) proc->commands[i].num_of_command);
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
    printf("commands:             command failed: " RED "%s" WHITE, all_commands_text[proc->commands[num_of_line].num_of_command]);
    print_commands(proc, num_of_line);
    printf("\nreg info:\n");
    print_regs(proc, num_of_line);
    printf(RED "\n\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/ENDING DUMP\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n\n" WHITE);
}

void procDtor(proc_t **proc) {
    if ((*proc)->stack != NULL)
        stackDtor((*proc)->stack);
    if ((*proc)->commands != NULL)
        free((*proc)->commands);
    if (*proc != NULL)
        free(*proc);
}

error_t write_file_to_proc(proc_t *proc, char * user_file_name) {
    sassert(proc,           ERR_PTR_NULL);
    sassert(user_file_name, ERR_PTR_NULL);
    if (!is_file_exists(user_file_name)) {
        add_error(ERR_FILE_DOES_NOT_EXIST, user_file_name);
        return error;
    }
    
    FILE *fp = fopen(user_file_name, "r");
    size_t file_size = get_file_size(fp);
    proc->num_of_lines = file_size / sizeof(line_format);
    if (proc->num_of_lines < 2) {
        add_error(ERR_FILE_SIZE_INCORRECT);
        return error;
    }

    fread(&(proc->header), sizeof(header_t), 1, fp);
    fread(proc->commands, sizeof(line_format), proc->num_of_lines - 1, fp);

    fclose(fp);
    return error;
}

error_t create_proc(proc_t *proc, const char * file_name, const char * func_name, size_t line) {
    sassert(file_name,      ERR_PTR_NULL);
    sassert(func_name,      ERR_PTR_NULL);
    sassert(proc,           ERR_PTR_NULL);

    for (size_t i = 0; i < num_of_regs; i++)
        proc->regs_values[i] = 0;
    init_stack(stack, 50);
    proc->stack = stack;
    proc->commands = (line_format *) calloc(50, sizeof(line_format));
    place_debug_info(proc);

    return error;
}

#define init_proc(proc) \
    proc_t *proc = (proc_t *) calloc(1, sizeof(proc_t));\
    create_proc(proc, __FILE__, __func__, __LINE__);

void print_help() {
    printf(MAGENTA "type 1) file which you need to execute\n" WHITE);
}

int main(int argc, char * argv[]) {
    if (argc     == 2) {
        init_proc(proc);
        write_file_to_proc(proc, argv[1]);
        execute_file(proc, false);
        procDtor(&proc);
    }
    else if (argc == 3 && strcmp(argv[2], "-d") == 0) {
        init_proc(proc);
        write_file_to_proc(proc, argv[1]);
        execute_file(proc, true);
        procDtor(&proc);
    }
    else 
        print_help();
    if (error.is_error == true) {
        print_error(error, error_text);
        return error.code;
    }
    printf(GREEN "file was successfully executed\n" WHITE);
    return 0;
}