#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_manage.h"
#include "sassert.h"
#include "assembly.h"
#include "../helpers/helpers.h"

calcInst_t get_num_of_command(const char * command) {
    sassert(command, ERR_PTR_NULL);

    for (size_t i = 0; i < sizeof(all_commands_text) / sizeof(string); i++) {
        if (strcmp(command, all_commands_text[i]) == 0) {
            return (calcInst_t) i;
        }
    }
    return UNDEF_CMD;
}

asmArg_t get_type_of_arg(calcInst_t num_of_command) {
    switch(num_of_command) {
        case PUSH:
        case POW:
        case JMP:
        case JB:    
        case JE:
        case JAE:
        case JNE:
        case JBE:
        case JA:
            return ONE_ARG_CMD;
        case PUSHR:
        case POPR:
            return REG_CMD;
        default:
            return ZERO_ARG_CMD;
    }
}

regs_enum get_reg_type(char * token_buffer) {
    sassert(token_buffer, ERR_PTR_NULL);

    for (size_t i = 0; i < sizeof(all_commands_text) / sizeof(string); i++) {
        if (strcmp(token_buffer, all_regs_str[i]) == 0) {
            return (regs_enum) i;
        }
    }
    return UNDEF_REG;
}

void place_command_argument(char **token_buffer, line_format *cur_line, asmArg_t how_many_args) {
    sassert(token_buffer,  ERR_PTR_NULL);
    sassert(*token_buffer, ERR_PTR_NULL);
    sassert(cur_line,      ERR_PTR_NULL);

    switch(how_many_args) {
        case ONE_ARG_CMD:
            *token_buffer   = strtok(NULL, " \t\n\r");
            cur_line->value = atof(*token_buffer);
            break;
        case REG_CMD:
            *token_buffer   = strtok(NULL, " \t\n\r");
            cur_line->value = (stack_var_t) get_reg_type(*token_buffer);
            break;
        case ZERO_ARG_CMD:
            cur_line->value = 0;
            break;
    }
}

void write_header(FILE *compiled_file) {
    header_t header = {SIGN, VERSION};
    fwrite(&header, sizeof(header_t), 1, compiled_file);
}

error_t compile_file(char * user_file_compile, char * user_file_where) {
    sassert(user_file_compile, ERR_PTR_NULL);
    sassert(user_file_where,   ERR_PTR_NULL);

    if (!is_file_exists(user_file_compile)) {
        add_error(ERR_FILE_DOES_NOT_EXIST, user_file_compile);
        return error;
    }
    
    FILE * compiled_file = fopen(user_file_where,  "wb");
    sassert(compiled_file, ERR_PTR_NULL);

    char * compile_buffer = get_buffer_from_file(user_file_compile);
    char * token_buffer   = strtok(compile_buffer, " \t\n\r");
    line_format cur_line  = {};
    write_header(compiled_file);

    while (token_buffer != NULL) {
        cur_line.num_of_command = get_num_of_command(token_buffer);

        if (cur_line.num_of_command == UNDEF_CMD) {
            add_error(ERR_UNDEFINED_CMD, token_buffer);
            break;
        }
        asmArg_t how_many_args = get_type_of_arg(cur_line.num_of_command);
        place_command_argument(&token_buffer, &cur_line, how_many_args);
        
        fwrite(&cur_line, sizeof(cur_line), 1, compiled_file);
        token_buffer = strtok(NULL, " \t\n\r");
    }
    free(compile_buffer);
    fclose(compiled_file);
    return error;
}

void print_help() {
    printf(MAGENTA "type 1) file you want to compile(needed)\n"
                   "     2) file where you want to compile(optional)\n" WHITE);
}

int main(int argc, char * argv[]) {
    if (argc == 2) {
        printf(MAGENTA "you did not type output file.\n"
                       "Compiling will be proceeded to <out.txt>\n" WHITE);
        char output_file[] = "out.txt";
        compile_file(argv[1], output_file);
    }
    else if (argc == 3) {
        compile_file(argv[1], argv[2]);
    }
    else {
        print_help();
        return 0;
    }

    if (error.is_error == true) {
            print_error(error, error_text);
            return error.code;
        }
    printf(GREEN "file was successfully compiled\n" WHITE);
    return 0;
}