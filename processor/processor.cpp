#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#include "error_manage.h"
#include "../helpers/helpers.h"
#include "../stack/stack.h"
#include "processor.h"
#include "processor_commands.h"

error_t error = {};

void do_long_skip(proc_t *proc, char *ch, size_t count)
{
    if (proc->commands[count].type_of_arg == LABEL_ARG_CMD)
    {
        procDump_internal(proc, count, "C:\\Users\\Azerty\\my_project\\processor\\processor\\processor.cpp", __func__, 17);
        if (getc(stdin) == 'm')
            *ch = 'm';
    }
}

void skip_and_dump(proc_t *proc, char *debug_mode, size_t count)
{
    if (*debug_mode == 'l')
    { // debug mode "l" dumps only at label type commands
        do_long_skip(proc, debug_mode, count);
    }
    else if (*debug_mode == 'm')
    { // debug mode "m" dumps at every command
        do_short_skip(proc, debug_mode, count);
        if (getc(stdin) == 'l')
            *debug_mode = 'l';
    }
}

void do_short_skip(proc_t *proc, char *ch, size_t count)
{
    procDump(proc, count);
}

error_t execute_file(proc_t *proc, bool debug_flag)
{
    sassert(proc, ERR_PTR_NULL);

    clock_t start = clock();
    size_t draw_clock = ns_now();
    size_t count = 0;
    bool is_first_draw = true;
    char debug_mode = 'm';
    if (proc->video.has_audio)
    {
        char command[MAX_STR_SIZE] = {};
        snprintf(command, MAX_STR_SIZE - 1, "play -q %s.wav &", proc->video.audio_file_name);
        system(command);
    }

    while (count < proc->num_of_lines)
    {
        CHECK_ERR_PROC(proc, proc->stack, count, 0);

        if (debug_flag == true)
            skip_and_dump(proc, &debug_mode, count);

        bool is_end = do_command(proc, &is_first_draw, &count, &draw_clock);
        if (is_end)
            break;
        count++;
    }

    printf("time taken(in seconds): %lf\n", (double)(clock() - start) / CLOCKS_PER_SEC);
    return error;
}

void procDtor(proc_t **proc)
{
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

    if ((*proc)->procRAM.procRAM != NULL)
        free((*proc)->procRAM.procRAM);

    free(*proc);
    END
}

char *skip_spaces(char *str)
{
    if (str == NULL)
        return NULL;
    while (*str == ' ' || *str == '\t')
    {
        str++;
    }
    return str;
}

error_t write_file_to_proc(proc_t *proc, char *user_file_name)
{
    sassert(proc, ERR_PTR_NULL);
    sassert(user_file_name, ERR_PTR_NULL);
    if (!is_file_exists(user_file_name))
    {
        add_error(ERR_FILE_DOES_NOT_EXIST, "%s", user_file_name);
        return error;
    }

    FILE *fp = fopen(user_file_name, "r");
    sassert(fp, ERR_PTR_NULL);

    size_t file_size = get_file_size(fp);
    fread(&(proc->header), sizeof(header_t), 1, fp);
    CHECK_PROC_HEADER(proc);

    fread(&(proc->num_of_lines), sizeof(size_t), 1, fp);
    if (proc->num_of_lines < 2)
    {
        add_error(ERR_FILE_SIZE_INCORRECT, "none");
        return error;
    }

    bool is_video = false;
    fread(&is_video, sizeof(bool), 1, fp);
    if (is_video)
    {
        proc->video.is_video = true;
        fread(&(proc->video.width), sizeof(size_t), 1, fp);
        fread(&(proc->video.height), sizeof(size_t), 1, fp);
        fread(&(proc->video.fps), sizeof(size_t), 1, fp);
        fread(&(proc->video.has_audio), sizeof(bool), 1, fp);

        if (proc->video.has_audio)
        {
            size_t len_audio_file = 0;
            fread(&len_audio_file, sizeof(size_t), 1, fp);
            fread(&(proc->video.audio_file_name), sizeof(char), len_audio_file, fp);
        }
    }

    fprintf(stderr, "reading %d lines from the file\n", proc->num_of_lines);
    proc->commands = (line_format *)calloc(proc->num_of_lines, sizeof(line_format));
    sassert(proc->commands, ERR_PTR_NULL);

    fread(proc->commands, sizeof(line_format), proc->num_of_lines, fp);
    fprintf(stderr, GREEN "done\n" WHITE);
    fclose(fp);
    return error;
}

error_t init_proc_internal(proc_t *proc, const char *file_name, const char *func_name, size_t line)
{
    sassert(file_name, ERR_PTR_NULL);
    sassert(func_name, ERR_PTR_NULL);
    sassert(proc, ERR_PTR_NULL);

    for (size_t i = 0; i < num_of_regs; i++)
        proc->regs_values[i] = 0;
    init_stack(stack, START_VAL);
    init_stack(return_stack, START_VAL);

    proc->procRAM.procRAM = (double *)calloc(START_VAL, sizeof(double));
    proc->procRAM.capacity = START_VAL;
    proc->return_stack = return_stack;
    proc->stack = stack;
    place_debug_info(proc);

    return error;
}

#define init_proc(proc)                                 \
    proc_t *proc = (proc_t *)calloc(1, sizeof(proc_t)); \
    init_proc_internal(proc, __FILE__, __func__, __LINE__);

void print_help()
{
    printf(MAGENTA "type 1) file which you need to execute\n" WHITE);
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        print_help();
        return 0;
    }
    init_proc(proc);

    bool needs_debug = false;
    if (argc == 3 && strcmp(argv[2], "-d") == 0)
        needs_debug = true;

    write_file_to_proc(proc, argv[1]);
    execute_file(proc, needs_debug);

    if (error.is_error == true)
    {
        print_error(error_text);
        return error.code;
    }

    printf(GREEN "file was successfully executed\n" WHITE);
    return 0;
}