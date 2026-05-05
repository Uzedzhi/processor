#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include "../stack/stack.h"
#include "sassert.h"
#include "error_manage.h"
#include "processor.h"
#include "../helpers/helpers.h"
#include "processor_commands.h"

extern error_t error;

void print_commands(proc_t *proc, size_t num_of_line)
{
    int start = num_of_line / CMD_RANGE * CMD_RANGE;
    int end = start + CMD_RANGE;
    if (end > proc->num_of_lines)
    {
        end = proc->num_of_lines;
        start -= end - proc->num_of_lines;
    }
    print_with_otstyp(otstyp, "    ");
    for (size_t i = start; i < end; i++)
    {
        if (i == num_of_line)
            printf(RED "[%02X] " WHITE, (unsigned int)proc->commands[num_of_line].num_of_command);
        else
            printf("[%02X] ", (unsigned int)proc->commands[i].num_of_command);
    }
    putchar('\n');
}

void print_regs(proc_t *proc, size_t num_of_line)
{
    print_with_otstyp(otstyp, "    ");
    for (size_t i = 0; i < num_of_regs; i++)
    {
        printf("[%07.2lf] ", (double)proc->regs_values[i]);
    }
    putchar('\n');
    print_with_otstyp(otstyp, "    ");
    for (size_t i = 0; i < num_of_regs; i++)
    {
        printf("   %s    ", all_regs_str[i]);
    }
    putchar('\n');
}

void print_next_command(size_t count, proc_t *proc, size_t num_of_line)
{
    sassert(proc, ERR_PTR_NULL);
    line_format cur_line = proc->commands[count];
    if (count == num_of_line)
        printf(GREEN "doing => \t%d) ", count);
    else
        printf("\t\t%d) ", count);
    switch (cur_line.type_of_arg)
    {
    case LABEL_ARG_CMD:
        printf("%-5s %lf\n", all_commands_text[cur_line.num_of_command], cur_line.cmd_arg);
        break;
    case REG_CMD:
        printf("%-5s %s\n", all_commands_text[cur_line.num_of_command], all_regs_str[(int)cur_line.cmd_arg]);
        break;
    case ONE_ARG_CMD:
        printf("%-5s %lf\n", all_commands_text[cur_line.num_of_command], cur_line.cmd_arg);
        break;
    case MEMORY_ARG_CMD:
        printf("%-5s [%d]\n", all_commands_text[cur_line.num_of_command], (int)cur_line.cmd_arg);
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

void print_memory_info(proc_t *proc, size_t num_of_line)
{
    for (size_t i = 0; i < min(MAX_MIN, proc->procRAM.capacity); i++)
    {
        if (i % 20 == 0)
        {
            printf("\n");
            print_with_otstyp(otstyp, "    ");
        }
        if (proc->procRAM.procRAM[i] != 0)
        {
            printf(YELLOW "[%02X]" WHITE, (unsigned int)proc->procRAM.procRAM[i]);
        }
        else
            printf("[%02X]", (unsigned int)proc->procRAM.procRAM[i]);
    }
}

void procDump_internal(proc_t *proc, size_t num_of_line, const char *file_name, const char *func_name, size_t line)
{
    int stack_errors = stackErrcheck(proc->stack, 0);
    stackDump(proc->stack, 0, 0, stack_errors, file_name, func_name, line);

    printf(MAGENTA "\n%sPROCESSOR DUMP%s\n" WHITE, format_string, format_string);

    for (size_t num = max(0, num_of_line - 4); num < min(num_of_line + 4, proc->num_of_lines); num++)
    {
        print_next_command(num, proc, num_of_line);
    }
    print_with_otstyp(otstyp, "num_of_lines: %zu\n", proc->num_of_lines);
    print_with_otstyp(otstyp, "init where     | <%s; %s:%zu>\n", proc->file_name, proc->func_name, proc->line);
    print_with_otstyp(otstyp, "dump where     | <%s; %s:%zu>\n", file_name, func_name, line);
    print_with_otstyp(otstyp, MAGENTA "commands:" WHITE "             command failed: " RED "%s %lf\n" WHITE, all_commands_text[proc->commands[num_of_line].num_of_command], (double)proc->commands[num_of_line].cmd_arg);
    print_commands(proc, num_of_line);
    print_with_otstyp(otstyp, MAGENTA "reg info:\n" WHITE);
    print_regs(proc, num_of_line);
    print_with_otstyp(otstyp, MAGENTA "return values info:" WHITE);
    STACKDUMP(proc->return_stack, 0, 1, 0);
    print_with_otstyp(otstyp, MAGENTA "memory info:" WHITE);
    print_with_otstyp(otstyp, "capacity: %zu\n", proc->procRAM.capacity);
    print_memory_info(proc, num_of_line);

    printf(RED "\n%sENDING DUMP%s\n" WHITE, format_string, format_string);
}