#ifndef HELPERS_H
#define HELPERS_H


#ifndef LEVEL_OF_CHECK
#define LEVEL_OF_CHECK 3
#endif // level_of_check

typedef const char * const string;
typedef double stack_var_t;
const size_t num_of_regs = 9;
const double VERSION = 1.0;
const size_t SIGN = 0xB333DEDDALL + 0xC0CAC0LL;

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
    ERR_FILE_SIZE_INCORRECT     = 12,
    ERR_INCORRECT_SIGN          = 13,
    ERR_INCORRECT_VERSION       = 14,
    FATAL_ERROR                 = 15,
    NO_ERROR                    = 16
};

#define CALC_INSTRUCTIONS(n) \
    n(PUSH, 0)      \
    n(POP, 1)       \
                    \
    n(ADD, 2)       \
    n(SUB, 3)       \
    n(MUL, 4)       \
    n(QROOT, 5)     \
    n(DIV, 6)       \
    n(POW, 7)       \
                    \
    n(PUSHR, 8)     \
    n(POPR, 9)      \
                    \
    n(IN, 10)       \
    n(DUMP, 11)     \
    n(OUT, 12)      \
    n(HLT, 13)      \
                    \
    n(JMP, 14)      \
    n(JB, 15)       \
    n(JE, 16)       \
    n(JA, 17)       \
    n(JBE, 18)      \
    n(JNE, 19)      \
    n(JAE, 20)      \
                    \
    n(UNDEF_CMD, 21)

#define CALC_REGS(n)    \
    n(RAX, 0)           \
    n(RBX, 1)           \
    n(RCX, 2)           \
    n(RDX, 3)           \
    n(REX, 4)           \
    n(TRX, 5)           \
    n(LLMV, 6)          \
    n(DED, 7)           \
    n(UNDEF_REG, 8)

#define INIT_ENUM_VAL(command, value) \
    command = value,
#define INIT_STR_ARRAY(command, value) \
    #command,
enum calcInst_t {
    CALC_INSTRUCTIONS(INIT_ENUM_VAL)
};
enum regs_enum {
    CALC_REGS(INIT_ENUM_VAL)
};
struct line_format {
    calcInst_t num_of_command;
    stack_var_t value;
};
struct header_t {
    size_t signature;
    double version;
};

string all_regs_str[num_of_regs] = {CALC_REGS(INIT_STR_ARRAY)};
const string error_text[] = {"your pointer is null", "stack is null", "capacity is invalid number", "size is invalid number", "type of your argument is different from initialized", "something changed region to the left of an array", "something changed region to the right of an array", "some number overflew past limit", "buffer size should be more than 0!", "hash of your function unexpectedly changed, maybe you swapped or edited elements by yourself?", "command is undefined", "file does not exist", "size is invalid", "signature is incorrect COPYRIGHT!!1!1", "Version is imcompatible", "fatal error, cant progress", "no error"};
const string all_commands_text[] = {CALC_INSTRUCTIONS(INIT_STR_ARRAY)};
const static size_t num_of_commands = sizeof(all_commands_text) / sizeof(string);

bool is_file_exists(const char * file_name);
void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read);
char * get_buffer_from_file(char * user_file_compile);
bool is_in_array(const char * command, size_t size);
size_t get_file_size(FILE * fp);
bool is_same(double a, double b);

#endif // helpers_h