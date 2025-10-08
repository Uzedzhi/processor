#ifndef assembly_h
#define assembly_h

#include "../helpers/helpers.h"

enum asmArg_t {
    ZERO_ARG_CMD, REG_CMD, ONE_ARG_CMD
};

calcInst_t get_num_of_command(const char * command);
asmArg_t get_type_of_arg(calcInst_t num_of_command);
regs_enum get_reg_type(char * token_buffer);
void place_command_argument(char **token_buffer, line_format *cur_line, asmArg_t how_many_args);
error_t compile_file(char * user_file_compile, char * user_file_where);
void print_help();
void write_header(FILE *compiled_file);


#endif // assembly_h