#ifndef HELPERS_H
#define HELPERS_H

typedef const char * const string;
typedef double stack_var_t;
const size_t num_of_regs = 8;
enum asmErr_t {
    ERR_PTR_NULL, ERR_STACK_NULL, ERR_CAPACITY_INVALID, ERR_SIZE_INVALID, ERR_DIFFERENT_TYPE, ERR_CANAREIKA_LEFT_CHANGE, ERR_CANAREIKA_RIGHT_CHANGE, ERR_OVERFLOW, ERR_BUFFER_SIZE_INVALID, ERR_HASH_CHANGED, ERR_UNDEFINED_CMD, ERR_FILE_DOES_NOT_EXIST, FATAL_ERROR, NO_ERROR
};

enum calcInst_t{
    PUSH_CMD, POP_CMD, ADD_CMD, SUB_CMD, MUL_CMD, QROOT_CMD, DIV_CMD, PUSHR_CMD, POPR_CMD, OUT_CMD, HLT_CMD, DUMP_CMD, UNDEF_CMD
};

struct line_format {
    calcInst_t num_of_command;
    stack_var_t value;
};

string all_regs_str[8] = {"RAX", "RBX", "RCX", "RDX", "REX", "RTX", "LLMV", "DED"};
const string error_text[] = {"your pointer is null", "stack is null", "capacity is invalid number", "size is invalid number", "type of your argument is different from initialized", "something changed region to the left of an array", "something changed region to the right of an array", "some number overflew past limit", "buffer size should be more than 0!", "hash of your function unexpectedly changed, maybe you swapped or edited elements by yourself?", "command is undefined", "file does not exist", "fatal error, cant progress", "no error"};

const static string all_commands[] = {"PUSH", "POP", "ADD", "SUB", "MUL", "QROOT", "DIV", "PUSHR", "POPR", "OUT", "HLT", "DUMP"};
const static size_t num_of_commands = sizeof(all_commands) / sizeof(string);

bool is_file_exists(const char * file_name);
void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read);
char * get_buffer_from_file(char * user_file_compile);
bool is_in_array(const char * command, size_t size);
size_t get_file_size(FILE * fp);
bool is_same(double a, double b);

#endif // helpers_h