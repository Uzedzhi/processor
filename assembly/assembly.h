#ifndef assembly_h
#define assembly_h

#include "../helpers/helpers.h"

#define die() \
    die_internal(__FILE__, __func__, __LINE__);

const static char COMMENT = ';';
const static char FIGURE_START = '{';
const static char FIGURE_END = '}';

struct asm_t {
    size_t num_of_lines;
    char * file_buffer;
    unsigned long hashes_of_cmd[num_of_commands];
    unsigned long hashes_of_regs[num_of_commands];
    line_format * bytecode_buffer;
    label labels[num_of_labels];
};

void place_label(asm_t *assembly, char * file_buffer, size_t num_of_line);
error_t place_command_argument(asm_t assembly, size_t cycle_count, char *file_buffer, stack_var_t *value, asmArg_t type_of_arg);
calcInst_t get_num_of_command(char * command);
error_t get_label_index(char * file_buffer, label labels[], size_t *label_value_index, size_t cycle_count);
regs_enum get_reg_type(char * token_buffer);
void place_label_name(char label_name[], char * file_buffer);
void print_help();


#endif // assembly_h