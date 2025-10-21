#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#include "../stack/stack.h"
#include "processor.h"
#include "../helpers/helpers.h"
#include "error_manage.h"

error_t do_push(proc_t *proc, stack_t *stack, size_t cur_line, stack_var_t value) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 0);
    stackPush(stack, value);

    return error;
}

error_t do_pop(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 1);
    
    stack_var_t a = 0;
    stackPop(stack, &a);
    return error;
}

error_t do_pushm(proc_t *proc, stack_t *stack, size_t cur_line, size_t value) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 0);
    if (value >= proc->procRAM->capacity) {
        add_error(ERR_SIZE_INVALID, GET_STR(value));
        return error;
    }

    stack_var_t a = proc->procRAM->values[(int)proc->regs_values[value]];
    stackPush(stack, a);
    return error;
}

stack_var_t get_array_r_value(array_r *array, size_t index) {
    return array->values[index];
}

error_t do_popm(proc_t *proc, stack_t *stack, size_t cur_line, size_t value) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 1);
    
    array_r *procRAM = proc->procRAM;
    int reg_value = (int) proc->regs_values[value];

    if (reg_value >= procRAM->capacity) {
        reallocate_array((void **) &procRAM->values, procRAM->capacity, reg_value * 2 * sizeof(stack_var_t));
        
        for (size_t i = procRAM->capacity; i < reg_value * 2; i++) {
            procRAM->values[i] = 0;
        }
        procRAM->capacity = reg_value * 2;
    }
    stackPop(stack, &(proc->procRAM->values[reg_value]));
    proc->procRAM->size = max(proc->procRAM->size, reg_value);
    return error;
}

error_t do_add(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a + b);
    return error;
}

error_t do_sub(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, b - a);
    return error;
}

error_t do_div(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, cur_line, 2);
    
    stack_var_t a = 0, b = 0;
    
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, b / a);
    return error;
}

error_t do_in(proc_t *proc, stack_t *stack, size_t num_of_line) {
    sassert(proc,  ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, num_of_line, 0);
 
    double a = 0;
    printf("type your number: ");
    scanf("%lf", &a);
    stackPush(stack, (stack_var_t) a);
    return error;
}


error_t do_draw(proc_t *proc, stack_t *stack, size_t num_of_line) {
    sassert(proc,  ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, num_of_line, 0);
    
    size_t capacity = proc->procRAM->capacity;
    size_t size     = proc->procRAM->size + 1;
    for (size_t i = 0; i < size; i++) {
        char value = (char) proc->procRAM->values[i];
        if (i % ((int) sqrt(4 * size)) == 0)
            putchar('\n');
        if (i % 2 == 0) 
            printf("%s", color_codes[value]);
        else {
            putchar(value);
            putchar(value);
        }
    }
    putchar('\n');
    return error;
}

error_t do_mod(proc_t *proc, stack_t *stack, size_t num_of_line) {
    sassert(proc,  ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, num_of_line, 2);

    stack_var_t a = 0;
    stack_var_t b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);

    stackPush(stack, (int) b % (int) a);
    return error;
}

error_t do_toint(proc_t *proc, stack_t *stack, size_t num_of_line) {
    sassert(proc,  ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, num_of_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    stackPush(stack, (int) a);

    return error;
}

error_t do_pow(proc_t *proc, stack_t *stack, size_t num_of_line, int value) {
    sassert(proc,  ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, num_of_line, 1);

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
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 2);

    stack_var_t a = 0, b = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPush(stack, a * b);
    return error;
}

error_t do_out(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    printf("==============<%lf>==============\n", (double) a);
    return error;
}

error_t do_jmp(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value) {
    sassert(proc, ERR_PTR_NULL);
    sassert(stack, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, *cur_line, 0);
    *cur_line = value - 1;
    return error;
}

error_t do_popr(proc_t *proc, stack_t *stack, size_t cur_line, int num_of_reg) {
    sassert(stack, ERR_PTR_NULL);
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    proc->regs_values[num_of_reg] = a;
    return error;
}

error_t do_jb(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value, calcInst_t num_of_command) {
    sassert(proc,     ERR_PTR_NULL);
    sassert(proc,     ERR_PTR_NULL)
    sassert(cur_line, ERR_PTR_NULL);
    CHECK_ERR_PROC(proc, stack, *cur_line, 2);

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
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 0);

    stackPush(stack, proc->regs_values[num_of_reg]);
    return error;
}

error_t do_det(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 3);

    stack_var_t a = 0;
    stack_var_t b = 0;
    stack_var_t c = 0;
    stackPop(stack, &a);
    stackPop(stack, &b);
    stackPop(stack, &c);
    stackPush(stack, b * b - 4 * a * c);
    return error;
}

error_t do_root(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 1);

    stack_var_t a = 0;
    stackPop(stack, &a);
    stackPush(stack, sqrt(a));
    return error;
}

error_t do_qroot(proc_t *proc, stack_t *stack, size_t cur_line) {
    sassert(stack, ERR_PTR_NULL);
    sassert(proc,  ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, cur_line, 3);
    
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

error_t do_call(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value) {
    sassert(stack,      ERR_PTR_NULL);
    sassert(proc,       ERR_PTR_NULL);
    sassert(cur_line,   ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, stack, *cur_line, 0);

    stackPush(proc->return_stack, *cur_line + 1);
    do_jmp(proc, proc->stack, cur_line, value);
    return error;
}

error_t do_ret(proc_t *proc, stack_t *stack, size_t *cur_line, size_t value) {
    sassert(stack,      ERR_PTR_NULL);
    sassert(proc,       ERR_PTR_NULL);
    sassert(cur_line,   ERR_PTR_NULL)
    CHECK_ERR_PROC(proc, proc->return_stack, *cur_line, 0);

    stack_var_t jmp_line = 0;
    stackPop(proc->return_stack, &jmp_line);
    *cur_line = jmp_line - 1;
    return error;
}


bool do_command(proc_t *proc, size_t *num_of_line, calcInst_t num_of_command, stack_var_t value) {
    sassert(proc,        ERR_PTR_NULL);
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
        case MOD:   do_mod(proc, proc->stack, *num_of_line);                            break;
        case ROOT:  do_root(proc, proc->stack, *num_of_line);                           break;
        case QROOT: do_qroot(proc, proc->stack, *num_of_line);                          break;
        case JMP:   do_jmp(proc, proc->stack, num_of_line, (size_t) value);             break;
        case PUSHR: do_pushr(proc, proc->stack, *num_of_line, (size_t) value);          break;
        case POPR:  do_popr(proc, proc->stack, *num_of_line, (size_t) value);           break;
        case PUSHM: do_pushm(proc, proc->stack, *num_of_line, (size_t) value);          break;
        case POPM:  do_popm(proc, proc->stack, *num_of_line, (size_t) value);           break;
        case DET:   do_det(proc, proc->stack, *num_of_line);                            break;
        case TOINT: do_toint(proc, proc->stack, *num_of_line);                          break;
        case CALL:  do_call(proc, proc->stack, num_of_line, (size_t) value);            break;
        case RET:   do_ret(proc, proc->stack, num_of_line,  (size_t) value);            break;
        case DRAW:  do_draw(proc, proc->stack, *num_of_line);                           break;
        case JE:
        case JA:
        case JAE:
        case JBE:
        case JNE:
        case JB:    do_jb(proc, proc->stack, num_of_line, value, num_of_command);       break; 
        case OUT:   do_out(proc, proc->stack, *num_of_line);                            break;
        case IN:    do_in(proc, proc->stack, *num_of_line);                             break;
        case DUMP:  do_dump(proc, *num_of_line);                                        break;
        case HLT:   procDtor(&proc);                                                    return true;
        case UNDEF_CMD: add_error(ERR_UNDEFINED_CMD, all_commands_text[num_of_command]);break;
        default:        push_error(FATAL_ERROR, "command code undefined");              break;
    }
    return false;

}

void do_long_skip(proc_t *proc, char *ch, size_t count) {
    if (proc->commands[count].type_of_arg == LABEL_ARG_CMD) {
        procDump(proc, count);
        if (getc(stdin) == 'm')
            *ch = 'm';
    }
}

void skip_and_dump(proc_t *proc, char *ch, size_t count) {
    if (*ch == 'l') {
        do_long_skip(proc, ch, count);
    }
    else if (*ch == 'm') {
        do_short_skip(proc, ch, count);
        if (getc(stdin) == 'l')
                *ch = 'l';
    }
}

void do_short_skip(proc_t *proc, char *ch, size_t count) {
    procDump(proc, count);
}

error_t execute_file(proc_t *proc, bool debug_flag) {
    sassert(proc, ERR_PTR_NULL);

    clock_t start = clock();
    size_t count = 0;
    char ch = 'm';

    while (count < proc->num_of_lines) {
        CHECK_ERR_PROC(proc, proc->stack, count, 0);

        if (debug_flag == true)
            skip_and_dump(proc, &ch, count);

        bool is_end = do_command(proc, &count, proc->commands[count].num_of_command, proc->commands[count].value);
        if (is_end)
            break;
        count++;
    }

    printf("time taken(in seconds): %lf\n", (double) (clock() - start) / CLOCKS_PER_SEC);
    return error;
}

void print_commands(proc_t *proc, size_t num_of_line) {
    int start = num_of_line / 10 * 10;
    int end = start + 10;
    if (end > proc->num_of_lines) {
        end = proc->num_of_lines;
        start -= end - proc->num_of_lines;
    }
    print_with_otstyp(otstyp, "    ");
    for (size_t i = start; i < end; i++) {
        if (i == num_of_line)
            printf(RED "[%02X] " WHITE, (unsigned int) proc->commands[num_of_line].num_of_command);
        else
            printf("[%02X] ", (unsigned int) proc->commands[i].num_of_command);
    }
    putchar('\n');
}

void print_regs(proc_t *proc, size_t num_of_line) {
    print_with_otstyp(otstyp, "    ");
    for (size_t i = 0; i < num_of_regs; i++) {
        printf("[%07.2lf] ", (double) proc->regs_values[i]);
    }
    putchar('\n');
    print_with_otstyp(otstyp, "    ");
    for (size_t i = 0; i < num_of_regs; i++) {
        printf("   %s    ", all_regs_str[i]);
    }
    putchar('\n');
}

void print_next_command(size_t count, proc_t *proc, size_t num_of_line) {
    sassert(proc, ERR_PTR_NULL);
    line_format cur_line = proc->commands[count];
    if (count == num_of_line)
        printf(GREEN "doing => \t%d) ", count);
    else
        printf("\t\t%d) ", count);
    switch(cur_line.type_of_arg) {
        case LABEL_ARG_CMD:
            printf("%-5s %lf\n", all_commands_text[cur_line.num_of_command], cur_line.value);
            break;
        case REG_CMD:
            printf("%-5s %s\n", all_commands_text[cur_line.num_of_command], all_regs_str[(int)cur_line.value]);
            break;
        case ONE_ARG_CMD:
            printf("%-5s %lf\n", all_commands_text[cur_line.num_of_command], cur_line.value);
            break;
        case MEMORY_ARG_CMD:
            printf("%-5s [%d]\n", all_commands_text[cur_line.num_of_command],  (int)cur_line.value);
            break;
        case ZERO_ARG_CMD:
            printf("%-5s\n", all_commands_text[cur_line.num_of_command]);
            break;
        default:
            printf("<err_cmd>\n");
            break;
    }
    printf(WHITE);
}

void print_memory_info(proc_t * proc, size_t num_of_line) {
    for (size_t i = 0; i < min(MAX_MIN, proc->procRAM->capacity); i++) {
        if (i % 20 == 0) {
            printf("\n");
            print_with_otstyp(otstyp, "    ");
        }
        if (proc->procRAM->values[i] != 0) {
            printf(YELLOW "[%02X]" WHITE, (unsigned int) proc->procRAM->values[i]);
        }
        else
            printf("[%02X]", (unsigned int) proc->procRAM->values[i]);
    }
}

void procDump_internal(proc_t *proc, size_t num_of_line, const char * file_name, const char * func_name, size_t line) {
    int stack_errors = stackErrcheck(proc->stack, 0);
    stackDump(proc->stack, 0, 0, stack_errors, file_name, func_name, line);

    printf(MAGENTA "\n%sPROCESSOR DUMP%s\n" WHITE, format_string, format_string);
    
    for (size_t num = max(0, num_of_line - 4); num < min(num_of_line + 4, proc->num_of_lines); num++) {
        print_next_command(num, proc, num_of_line);
    }
    print_with_otstyp(otstyp, "num_of_lines: %zu\n", proc->num_of_lines);
    print_with_otstyp(otstyp, "init where     | <%s; %s:%zu>\n",        proc->file_name, proc->func_name, proc->line);
    print_with_otstyp(otstyp, "dump where     | <%s; %s:%zu>\n",        file_name, func_name, line);
    print_with_otstyp(otstyp, MAGENTA "commands:" WHITE "             command failed: " RED "%s %lf\n" WHITE, all_commands_text[proc->commands[num_of_line].num_of_command], (double) proc->commands[num_of_line].value);
    print_commands(proc, num_of_line);
    print_with_otstyp(otstyp, MAGENTA "reg info:\n" WHITE);
    print_regs(proc, num_of_line);
    print_with_otstyp(otstyp, MAGENTA "return values info:" WHITE);
    STACKDUMP(proc->return_stack, 0, 1, 0);
    print_with_otstyp(otstyp, MAGENTA "memory info:" WHITE);
    print_with_otstyp(otstyp, "capacity: %zu\n", proc->procRAM->capacity);
    print_memory_info(proc, num_of_line);

    printf(RED "\n%sENDING DUMP%s\n" WHITE, format_string, format_string);
}

void procDtor(proc_t **proc) {
    sassert(proc, ERR_PTR_NULL);
    
    BEGIN

    if (*proc == NULL)
        break;

    if ((*proc)->stack != NULL)
        stackDtor(((*proc)->stack));
    
    if ((*proc)->commands != NULL)
        free((*proc)->commands);
    
    if ((*proc)->return_stack != NULL)
        stackDtor((*proc)->return_stack);
    
    if ((*proc)->procRAM->values != NULL)
        free((*proc)->procRAM->values);
    
    if ((*proc)->procRAM != NULL)
        free((*proc)->procRAM);
    
    free(*proc);
    END
    
}

char * skip_spaces(char * str) {
    if (str == NULL)
        return NULL;
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}

error_t write_file_to_proc(proc_t *proc, char * user_file_name) {
    sassert(proc,           ERR_PTR_NULL);
    sassert(user_file_name, ERR_PTR_NULL);
    if (!is_file_exists(user_file_name)) {
        add_error(ERR_FILE_DOES_NOT_EXIST, user_file_name);
        return error;
    }
    
    FILE *fp = fopen(user_file_name, "r");
    sassert(fp, ERR_PTR_NULL);

    size_t file_size = get_file_size(fp);
    fread(&(proc->header), sizeof(header_t), 1, fp);
    CHECK_PROC_HEADER(proc);
    
    fread(&(proc->num_of_lines), sizeof(size_t), 1, fp);
    if (proc->num_of_lines < 2) {
        add_error(ERR_FILE_SIZE_INCORRECT);
        return error;
    }

    proc->commands = (line_format *) calloc(proc->num_of_lines, sizeof(line_format));
    sassert(proc->commands, ERR_PTR_NULL);

    fread(proc->commands, sizeof(line_format), proc->num_of_lines, fp);
    fclose(fp);
    return error;
}

#define init_array_r(name, capacy) \
    array_r *name = (array_r *) calloc(1, sizeof(array_r));\
    name->values = (stack_var_t *) calloc(capacy, sizeof(stack_var_t));\
    name->size = 0;\
    name->capacity = capacy;

error_t init_proc_internal(proc_t *proc, const char * file_name, const char * func_name, size_t line) {
    sassert(file_name,      ERR_PTR_NULL);
    sassert(func_name,      ERR_PTR_NULL);
    sassert(proc,           ERR_PTR_NULL);

    for (size_t i = 0; i < num_of_regs; i++)
        proc->regs_values[i] = 0;
    init_stack(stack, START_VAL);
    init_stack(return_stack, START_VAL);
    init_array_r(procRAM, START_VAL);
    proc->procRAM = procRAM;
    proc->return_stack = return_stack;
    proc->stack = stack;
    place_debug_info(proc);

    return error;
}

#define init_proc(proc) \
    proc_t *proc = (proc_t *) calloc(1, sizeof(proc_t));\
    init_proc_internal(proc, __FILE__, __func__, __LINE__);

void print_help() {
    printf(MAGENTA "type 1) file which you need to execute\n" WHITE);
}

int main(int argc, char * argv[]) {
    if (argc < 2 || argc > 3) {
        print_help();
        return 0;
    }
    init_proc(proc);

    bool needs_debug = false;
    if (argc == 3 && strcmp(argv[2], "-d") == 0) 
        needs_debug = true;


    write_file_to_proc(proc, argv[1]);
    execute_file(proc, needs_debug);

    if (error.is_error == true) {
        print_error(error, error_text);
        return error.code;
    }

    printf(GREEN "file was successfully executed\n" WHITE);
    return 0;
}