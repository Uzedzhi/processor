#ifndef HELPERS_H
#define HELPERS_H

typedef const char * const string;
typedef double stack_var_t;
const size_t num_of_regs = 8;

enum asmErr_t {
    ERR_PTR_NULL                = 0,
    ERR_STACK_NULL              = 1,
    ERR_CAPACITY_INVALID        = 2,
    ERR_SIZE_INVALID            = 3,
    ERR_DIFFERENT_TYPE          = 4,
    ERR_CANAREIKA_LEFT_CHANGE   = 5,
    ERR_CANAREIKA_RIGHT_CHANGE  = 6,
    ERR_OVERFLOW                = 7,
    ERR_BUFFER_SIZE_INVALID     = 8,
    ERR_HASH_CHANGED            = 9,
    ERR_UNDEFINED_CMD           = 10,
    ERR_FILE_DOES_NOT_EXIST     = 11,
    FATAL_ERROR                 = 12,
    NO_ERROR                    = 13
};

enum calcInst_t {
    PUSH_CMD  =       0,
    POP_CMD   =       1,
    ADD_CMD   =       2,
    SUB_CMD   =       3,
    MUL_CMD   =       4,
    QROOT_CMD =       5,
    DIV_CMD   =       6,
    PUSHR_CMD =       7,
    POPR_CMD  =       8,
    OUT_CMD   =       9,
    HLT_CMD   =       10,
    DUMP_CMD  =       11,
    JMP_CMD   =       12,
    JB_CMD    =       13,
    JE_CMD    =       14,
    JA_CMD    =       15,
    JBE_CMD   =       16,
    JNE_CMD   =       17,
    JAE_CMD   =       18,
    UNDEF_CMD =       19,
    IN_CMD    =       20,
};

struct line_format {
    calcInst_t num_of_command;
    stack_var_t value;
};

string all_regs_str[8] = {"RAX", "RBX", "RCX", "RDX", "REX", "RTX", "LLMV", "DED"};
const string error_text[] = {"your pointer is null", "stack is null", "capacity is invalid number", "size is invalid number", "type of your argument is different from initialized", "something changed region to the left of an array", "something changed region to the right of an array", "some number overflew past limit", "buffer size should be more than 0!", "hash of your function unexpectedly changed, maybe you swapped or edited elements by yourself?", "command is undefined", "file does not exist", "fatal error, cant progress", "no error"};

const static string all_commands[] = {"PUSH", "POP", "ADD", "SUB", "MUL", "QROOT",
                                      "DIV", "PUSHR", "POPR", "OUT", "HLT", "DUMP",
                                      "JMP", "JB", "JE", "JA", "JBE", "JNE", "JAE",
                                      "UNDEF", "IN"};
const static size_t num_of_commands = sizeof(all_commands) / sizeof(string);

bool is_file_exists(const char * file_name);
void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read);
char * get_buffer_from_file(char * user_file_compile);
bool is_in_array(const char * command, size_t size);
size_t get_file_size(FILE * fp);
bool is_same(double a, double b);

#endif // helpers_h