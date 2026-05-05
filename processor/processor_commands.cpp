#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>

#include "../stack/stack.h"
#include "sassert.h"
#include "error_manage.h"
#include "processor.h"
#include "../helpers/helpers.h"
#include "processor_commands.h"

extern error_t error;

#define do_push(proc, stack, cur_line, command_arg) \
    {                                               \
        CHECK_ERR_PROC(proc, stack, cur_line, 0);   \
        stackPush(stack, command_arg);              \
    }

#define do_pop(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 1); \
        stack_var_t a = 0;                        \
        stackPop(stack, &a);                      \
    }

#define do_add(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 2); \
        stack_var_t a = 0;                        \
        stack_var_t b = 0;                        \
        stackPop(stack, &a);                      \
        stackPop(stack, &b);                      \
        stackPush(stack, a + b);                  \
    }

#define do_sub(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 2); \
        stack_var_t a = 0;                        \
        stack_var_t b = 0;                        \
        stackPop(stack, &a);                      \
        stackPop(stack, &b);                      \
        stackPush(stack, b - a);                  \
    }

#define do_div(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 2); \
        stack_var_t a = 0;                        \
        stack_var_t b = 0;                        \
        stackPop(stack, &a);                      \
        stackPop(stack, &b);                      \
        stackPush(stack, b / a);                  \
    }

#define do_in(proc, stack, num_of_line)              \
    {                                                \
        CHECK_ERR_PROC(proc, stack, num_of_line, 0); \
        printf("type your number: ");                \
        stack_var_t a = 0;                           \
        scanf("%lf", &a);                            \
        stackPush(stack, a);                         \
    }

#define do_mod(proc, stack, num_of_line)             \
    {                                                \
        CHECK_ERR_PROC(proc, stack, num_of_line, 2); \
        stack_var_t a = 0;                           \
        stack_var_t b = 0;                           \
        stackPop(stack, &a);                         \
        stackPop(stack, &b);                         \
        stackPush(stack, (int)b % (int)a);           \
    }

#define do_toint(proc, stack, num_of_line) \
    {                                      \
        stack_var_t a = 0;                 \
        stackPop(stack, &a);               \
        stackPush(stack, (int)a);          \
    }

#define do_pow(proc, stack, num_of_line, command_arg) \
    {                                                 \
        CHECK_ERR_PROC(proc, stack, num_of_line, 1);  \
        stack_var_t a = 0;                            \
        stackPop(stack, &a);                          \
        a = (stack_var_t)pow(a, command_arg);         \
        stackPush(stack, a);                          \
    }

#define do_dump(proc, num_of_line)   \
    {                                \
        procDump(proc, num_of_line); \
    }

#define do_mul(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 2); \
        stack_var_t a = 0;                        \
        stack_var_t b = 0;                        \
        stackPop(stack, &a);                      \
        stackPop(stack, &b);                      \
        stackPush(stack, a * b);                  \
    }

#define do_out(proc, stack, cur_line)                             \
    {                                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 1);                 \
        stack_var_t a = 0;                                        \
        stackPop(stack, &a);                                      \
        printf("==============<%lf>==============\n", (double)a); \
    }

#define do_jmp(proc, stack, cur_line, command_arg) \
    {                                              \
        CHECK_ERR_PROC(proc, stack, *cur_line, 0); \
        *cur_line = command_arg - 1;               \
    }

#define do_popr(proc, stack, cur_line, num_of_reg) \
    {                                              \
        CHECK_ERR_PROC(proc, stack, cur_line, 1);  \
        stack_var_t a = 0;                         \
        stackPop(stack, &a);                       \
        proc->regs_values[num_of_reg] = a;         \
    }

#define do_jb(proc, stack, cur_line, command_arg, num_of_command) \
    {                                                             \
        CHECK_ERR_PROC(proc, stack, *(cur_line), 2);              \
        stack_var_t a = 0;                                        \
        stack_var_t b = 0;                                        \
        stackPop(stack, &a);                                      \
        stackPop(stack, &b);                                      \
        switch (num_of_command)                                   \
        {                                                         \
        case JA:                                                  \
            if (b > a)                                            \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        case JB:                                                  \
            if (b < a)                                            \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        case JE:                                                  \
            if (is_same(b, a))                                    \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        case JAE:                                                 \
            if (b >= a)                                           \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        case JBE:                                                 \
            if (b <= a)                                           \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        case JNE:                                                 \
            if (!is_same(b, a))                                   \
                *(cur_line) = (size_t)command_arg - 1;            \
            break;                                                \
        default:                                                  \
            break;                                                \
        }                                                         \
    }

#define do_pushr(proc, stack, cur_line, num_of_reg)      \
    {                                                    \
        CHECK_ERR_PROC(proc, stack, cur_line, 0);        \
        stackPush(stack, proc->regs_values[num_of_reg]); \
    }

#define do_det(proc, stack, cur_line)             \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 3); \
        stack_var_t a = 0;                        \
        stack_var_t b = 0;                        \
        stack_var_t c = 0;                        \
        stackPop(stack, &a);                      \
        stackPop(stack, &b);                      \
        stackPop(stack, &c);                      \
        stackPush(stack, b * b - 4 * a * c);      \
    }

#define do_root(proc, stack, cur_line)            \
    {                                             \
        CHECK_ERR_PROC(proc, stack, cur_line, 1); \
        stack_var_t a = 0;                        \
        stackPop(stack, &a);                      \
        stackPush(stack, sqrt(a));                \
    }

#define do_qroot(proc, stack, cur_line)                 \
    {                                                   \
        CHECK_ERR_PROC(proc, stack, cur_line, 3);       \
        stack_var_t a = 0;                              \
        stack_var_t b = 0;                              \
        stack_var_t c = 0;                              \
        stackPop(stack, &c);                            \
        stackPop(stack, &b);                            \
        stackPop(stack, &a);                            \
        double D = b * b - 4 * a * c;                   \
        if (D > 0)                                      \
        {                                               \
            stackPush(stack, (-b + sqrt(D)) / (2 * a)); \
            stackPush(stack, (-b - sqrt(D)) / (2 * a)); \
        }                                               \
        else if (is_same(D, 0))                         \
        {                                               \
            stackPush(stack, -b / (2 * a));             \
        }                                               \
        else                                            \
        {                                               \
            stackPush(stack, 0);                        \
        }                                               \
    }

#define do_call(proc, stack, cur_line, command_arg)   \
    {                                                 \
        CHECK_ERR_PROC(proc, stack, *cur_line, 0);    \
        stackPush(proc->return_stack, *cur_line + 1); \
        do_jmp(proc, stack, cur_line, command_arg);   \
    }

#define do_ret(proc, stack, cur_line, command_arg)              \
    {                                                           \
        CHECK_ERR_PROC(proc, proc->return_stack, *cur_line, 0); \
        stack_var_t jmp_line = 0;                               \
        stackPop(proc->return_stack, &jmp_line);                \
        *cur_line = jmp_line - 1;                               \
    }

#define do_pushm(proc, stack, cur_line, command_arg)           \
    {                                                          \
        CHECK_ERR_PROC(proc, stack, cur_line, 0);              \
                                                               \
        int reg_value = 0;                                     \
        if (command_arg >= 9)                                  \
        {                                                      \
            reg_value = command_arg;                           \
        }                                                      \
        else                                                   \
        {                                                      \
            reg_value = (int) proc->regs_values[command_arg];   \
        }                                                      \
        if (command_arg >= proc->procRAM.capacity)             \
        {                                                      \
            add_error(ERR_SIZE_INVALID, GET_STR(command_arg)); \
            break;                                             \
        }                                                      \
                                                               \
        stack_var_t a = proc->procRAM.procRAM[reg_value];      \
        stackPush(stack, a);                                   \
    }

#define do_pushmn(proc, stack, cur_line, command_arg)          \
    {                                                          \
        CHECK_ERR_PROC(proc, stack, cur_line, 0);              \
                                                               \
        if (command_arg >= proc->procRAM.capacity)             \
        {                                                      \
            add_error(ERR_SIZE_INVALID, GET_STR(command_arg)); \
            break;                                             \
        }                                                      \
                                                               \
        stack_var_t a = proc->procRAM.procRAM[command_arg];    \
        stackPush(stack, a);                                   \
    }

#define do_popm(proc, stack, cur_line, command_arg)                                                                           \
    {                                                                                                                         \
        CHECK_ERR_PROC(proc, stack, cur_line, 1);                                                                             \
                                                                                                                              \
        int reg_value = reg_value = (int)proc->regs_values[command_arg];                                                      \
        if (reg_value >= proc->procRAM.capacity)                                                                              \
        {                                                                                                                     \
                                                                                                                              \
            reallocate_array((void **)&proc->procRAM.procRAM, proc->procRAM.capacity, reg_value * 3 * sizeof(unsigned long)); \
            for (size_t i = proc->procRAM.capacity; i < reg_value * 3; i++)                                                   \
            {                                                                                                                 \
                proc->procRAM.procRAM[i] = 0;                                                                                 \
            }                                                                                                                 \
            proc->procRAM.capacity = reg_value * 3;                                                                           \
        }                                                                                                                     \
        stack_var_t a = 0;                                                                                                    \
        stackPop(stack, &a);                                                                                                  \
        proc->procRAM.procRAM[reg_value] = a;                                                                  \
        proc->procRAM.size = max(proc->procRAM.size, reg_value);                                                              \
    }

#define do_popmn(proc, stack, cur_line, command_arg)                                                                          \
    {                                                                                                                         \
        CHECK_ERR_PROC(proc, stack, cur_line, 1);                                                                             \
                                                                                                                              \
        int reg_value = command_arg;                                                                                          \
        if (reg_value >= proc->procRAM.capacity)                                                                              \
        {                                                                                                                     \
                                                                                                                              \
            reallocate_array((void **)&proc->procRAM.procRAM, proc->procRAM.capacity, reg_value * 3 * sizeof(unsigned long)); \
            for (size_t i = proc->procRAM.capacity; i < reg_value * 3; i++)                                                   \
            {                                                                                                                 \
                proc->procRAM.procRAM[i] = 0;                                                                                 \
            }                                                                                                                 \
            proc->procRAM.capacity = reg_value * 3;                                                                           \
        }                                                                                                                     \
        stack_var_t a = 0;                                                                                                    \
        stackPop(stack, &a);                                                                                                  \
        proc->procRAM.procRAM[reg_value] = a;                                                                           \
        proc->procRAM.size = max(proc->procRAM.size, reg_value);                                                              \
    }

size_t fps_to_nsecs(size_t fps)
{
    return 1e9 / fps;
}

size_t ns_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (size_t)ts.tv_sec * 1e9 + (size_t)ts.tv_nsec;
}

size_t get_fps_error(size_t fps)
{
    return 10000 + fps * 300;
}

void print_frame(proc_t *proc, size_t width, size_t height)
{
    printf("\033[1;1H");
    for (size_t i = 0; i < width * height; i++)
    {
        unsigned char *color = (unsigned char *)&proc->procRAM.procRAM[i];
        if (i % width == 0)
        {
            putchar('\n');
        }
        printf("\033[48;2;%d;%d;%dm  ", color[0], color[1], color[2]);
    }
    putchar('\n');
}

void print_memory(proc_t *proc)
{
    size_t square_side = 2 * (int)sqrt((proc->procRAM.size + 1) / 2);
    for (size_t i = 0; i < proc->procRAM.size + 1; i++)
    {
        unsigned long value = proc->procRAM.procRAM[i];
        if (i % square_side == 0)
        {
            putchar('\n');
        }
        if (i % 2 == 0)
        {
            printf("%s", color_codes[value]);
        }
        else
        {
            putchar(value);
            putchar(value);
        }
    }
}

#define do_draw(proc, stack, num_of_line, draw_clock, is_first_draw)   \
    {                                                                  \
        CHECK_ERR_PROC(proc, stack, num_of_line, 0);                   \
        const size_t frame_ns = 1e9 / (size_t)((proc)->video.fps);     \
                                                                       \
        if (!(*is_first_draw))                                         \
        {                                                              \
            size_t diff_ns = ns_now() - *(draw_clock);                 \
            if (diff_ns < frame_ns)                                    \
            {                                                          \
                size_t wait_ns = (size_t)((frame_ns - diff_ns) / 1.1); \
                struct timespec req = {                                \
                    .tv_sec = (time_t)(wait_ns / (time_t)(1e9)),       \
                    .tv_nsec = (long)(wait_ns % ((size_t)1e9))};       \
                struct timespec rem = {0, 0};                          \
                while (nanosleep(&req, &rem) == -1)  \
                {                                                      \
                    req = rem;                                         \
                }                                                      \
            }                                                          \
        }                                                              \
        else                                                           \
        {                                                              \
            *draw_clock = ns_now();                                    \
            *is_first_draw = false;                                    \
        }                                                              \
                                                                       \
        if (proc->video.is_video)                                      \
        {                                                              \
            size_t width = proc->video.width;                          \
            size_t height = proc->video.height;                        \
            size_t fps = proc->video.fps;                              \
            print_frame(proc, width, height);                          \
        }                                                              \
        else                                                           \
        {                                                              \
            print_memory(proc);                                        \
        }                                                              \
        *draw_clock = ns_now();                                        \
    }

bool do_command(proc_t *proc, bool *is_first_draw, size_t *num_of_line, size_t *draw_clock)
{
    sassert(proc, ERR_PTR_NULL);
    sassert(proc->stack, ERR_PTR_NULL);
    sassert(num_of_line, ERR_PTR_NULL);
    sassert(is_first_draw, ERR_PTR_NULL);
    sassert(draw_clock, ERR_PTR_NULL);
    calcInst_t num_of_command = proc->commands[*num_of_line].num_of_command;
    stack_var_t command_arg = proc->commands[*num_of_line].cmd_arg;
    switch (num_of_command)
    {
    case PUSH:
        do_push(proc, proc->stack, *num_of_line, command_arg);
        break;
    case POP:
        do_pop(proc, proc->stack, *num_of_line);
        break;
    case ADD:
        do_add(proc, proc->stack, *num_of_line);
        break;
    case SUB:
        do_sub(proc, proc->stack, *num_of_line);
        break;
    case DIV:
        do_div(proc, proc->stack, *num_of_line);
        break;
    case MUL:
        do_mul(proc, proc->stack, *num_of_line);
        break;
    case POW:
        do_pow(proc, proc->stack, *num_of_line, command_arg);
        break;
    case MOD:
        do_mod(proc, proc->stack, *num_of_line);
        break;
    case ROOT:
        do_root(proc, proc->stack, *num_of_line);
        break;
    case QROOT:
        do_qroot(proc, proc->stack, *num_of_line);
        break;
    case JMP:
        do_jmp(proc, proc->stack, num_of_line, (size_t)command_arg);
        break;
    case PUSHR:
        do_pushr(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case POPR:
        do_popr(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case PUSHM:
        do_pushm(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case PUSHMN:
        do_pushmn(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case POPM:
        do_popm(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case POPMN:
        do_popmn(proc, proc->stack, *num_of_line, (size_t)command_arg);
        break;
    case DET:
        do_det(proc, proc->stack, *num_of_line);
        break;
    case TOINT:
        do_toint(proc, proc->stack, *num_of_line);
        break;
    case CALL:
        do_call(proc, proc->stack, num_of_line, (size_t)command_arg);
        break;
    case RET:
        do_ret(proc, proc->stack, num_of_line, (size_t)command_arg);
        break;
    case DRAW:
        do_draw(proc, proc->stack, *num_of_line, draw_clock, is_first_draw);
        break;
    case JE:
    case JA:
    case JAE:
    case JBE:
    case JNE:
    case JB:
        do_jb(proc, proc->stack, num_of_line, command_arg, num_of_command);
        break;
    case OUT:
        do_out(proc, proc->stack, *num_of_line);
        break;
    case IN:
        do_in(proc, proc->stack, *num_of_line);
        break;
    case DUMP:
        do_dump(proc, *num_of_line);
        break;
    case HLT:
        procDtor(&proc);
        return true;
    case UNDEF_CMD:
        add_error(ERR_UNDEFINED_CMD, "%s", all_commands_text[num_of_command]);
        break;
    default:
        push_error(FATAL_ERROR, "command code undefined");
        break;
    }
    return false;
}