#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "programtoasm.h"
#include "better_output.h"
#include "sassert.h"

const size_t MAX_STR_SIZE = 500;

#define ASM_OPER_TYPES(n) \
    n(IF,               "IF"    )\
    n(EQ,               "=="    )\
    n(LEQ,              "<="    )\
    n(MEQ,              ">="    )\
    n(LESS,             "<"     )\
    n(MORE,             ">"     )\
    n(NEQ,              "!="    )\
    n(ASSIGN,           "="     )\
    n(OUTPUT,           "OUTPUT"   )\
    n(VAR_DECLARATION,  "VAR_DECLARATION")\
    n(FUNC_CALL,        "FUNC_CALL")\
    n(LITERAL,          "LITERAL")\
    n(WHILE,            "WHILE" )\
    n(STR,              "STR"   )\
    n(ARTHM_MUL,        "*"     )\
    n(ARTHM_ADD,        "+"     )\
    n(ARTHM_SUB,        "-"     )\
    n(ARTHM_DIV,        "/"     )\
    n(ARTHM_SIN,        "sin"   )\
    n(ARTHM_COS,        "cos"   )\
    n(ARTHM_TG,         "tg"    )\
    n(ARTHM_CTG,        "ctg"   )\
    n(ARTHM_POW,        "pow"   )\
    n(ARTHM_SQRT,       "sqrt"  )\
    n(ARTHM_LN,         "ln"    )\
    n(ARTHM_LOG,        "log"   )\
    n(ARTHM_SH,         "sh"    )\
    n(ARTHM_CH,         "ch"    )\
    n(ARTHM_CTH,        "cth"   )\
    n(ARTHM_TH,         "th"    )\
    n(ARTHM_ARCSIN,     "arcsin")\
    n(ARTHM_ARCCOS,     "arccos")\
    n(ARTHM_ARCTG,      "arctg" )\
    n(ARTHM_ARCCTG,     "arcctg")

#define INIT_OPER_STR(name, ...) \
    __VA_ARGS__,
#define INIT_OPER_ENUM(name, ...) \
    name,

enum AsmErr_t {
    ERR_PTR_NULL = 0
};

enum AsmOper_t {
    NOP = -1,
    ASM_OPER_TYPES(INIT_OPER_ENUM)
};

const char * const AllAsmOperStr[] = {ASM_OPER_TYPES(INIT_OPER_STR)};
size_t NumOpers = sizeof(AllAsmOperStr) / sizeof(*AllAsmOperStr);

enum LangType_e {
    NTYPE           =  -1,
    TYPE_OP         =   0,
    TYPE_VAR        =   1,
    TYPE_NUM        =   2,
    TYPE_CNOP       =   3,
    TYPE_STR        =   4
};

union LangElem_u {
    AsmOper_t oper;
    char *var_name;
    int num;
};

typedef struct Node_t {
    LangType_e type;
    LangElem_u value;
    Node_t * left;
    Node_t * right;
} Node_t;


#define CALLOC_WITH_TYPE(num_of_elements, type) \
    (type *) calloc(num_of_elements, sizeof(type))

void PrintHelp() {
    printf(MAGENTA "type 1) file you want to compile\n"
                   "     2) file as output\n" WHITE);
}

AsmOper_t CheckOperType(const char * const FileBuf, size_t *str_len) {
    sassert(FileBuf, ERR_PTR_NULL);

    for (size_t i = 0; i < NumOpers; i++) {
        size_t CurStrlen = strlen(AllAsmOperStr[i]);
        if (strncmp(FileBuf, AllAsmOperStr[i], CurStrlen) == 0) {
            *str_len = CurStrlen;
            return (AsmOper_t) i;
        }
    }

    return NOP;
}

size_t get_file_size(FILE * fp) {
    sassert(fp, ERR_PTR_NULL);

    fseek(fp, 0, SEEK_END);
    size_t file_size = (size_t) ftell(fp);
    rewind(fp);

    return file_size;
}

void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read) {
    sassert(buffer != NULL, ERR_PTR_NULL);

    while (actually_read < file_size) {
        buffer[++actually_read] = '\0';
    }
}

char * get_buffer_from_file(FILE * fp, size_t file_size) {
    sassert(fp, ERR_PTR_NULL);

    char * compile_buffer = (char *) calloc(file_size + 1, sizeof(char));
    sassert(compile_buffer, ERR_PTR_NULL);

    size_t actually_read = fread(compile_buffer, sizeof(char), file_size, fp);
    nullify_anything_extra(compile_buffer, file_size, actually_read);

    return compile_buffer;
}



static void SkipSpaces(size_t * pos, const char * buffer) {
    while (isspace(buffer[*pos]))
        (*pos)++;
}

static char * ReadQuotedString(size_t *pos, const char *buffer) {
    (*pos)++; // skip first "
    size_t start = *pos;
    while (buffer[*pos] != '\"')
        (*pos)++;

    size_t len = *pos - start;
    char *str = CALLOC_WITH_TYPE(len + 1, char);
    strncpy(str, buffer + *pos, len);

    (*pos)++; // skip last "
    return str;
}

Node_t * create_node() {
    return CALLOC_WITH_TYPE(1, Node_t);
}

Node_t * NewNode(LangType_e type, LangElem_u value, Node_t *left, Node_t *right) {
    Node_t * node = create_node();
    sassert(node, ERR_PTR_NULL);


    node->type  = type;
    node->value = value;
    node->left  = left;
    node->right = right;
    return node;
}

static bool IsIntStr(const char *s) {
    if (*s == '-') s++;
    if (!isdigit(*s)) return false;
    while (*s) {
        if (!isdigit(*s)) return false;
        s++;
    }
    return true;
}

static Node_t * BuildAsmTreeInternal(const char *buffer, size_t *pos) {
    SkipSpaces(pos, buffer);

    (*pos)++; // skip '('
    SkipSpaces(pos, buffer);

    if (buffer[*pos] == '\"') {
        SkipSpaces(pos, buffer);
        char *str = ReadQuotedString(pos, buffer);
        SkipSpaces(pos, buffer);

        Node_t *node = NULL;
        if (IsIntStr(str)) {
            node = NewNode(TYPE_NUM, (LangElem_u){.num = atoi(str)}, NULL, NULL);
            free(str);
        }
        else {
            node = NewNode(TYPE_VAR, (LangElem_u){.var_name = str}, NULL, NULL);
        }

        SkipSpaces(pos, buffer);
        (*pos)++; // skip ')'
        return node;
    }

    // OPER
    size_t str_len = 0;
    AsmOper_t oper = CheckOperType(buffer, &str_len);
    (*pos) += str_len;

    Node_t *node = NewNode(TYPE_OP, (LangElem_u){.oper = oper}, NULL, NULL);
    SkipSpaces(pos, buffer);

    // left
    if (buffer[*pos] == '(')
        node->left = BuildAsmTreeInternal(buffer, pos);

    SkipSpaces(pos, buffer);

    // right
    if (buffer[*pos] == '(')
        node->right = BuildAsmTreeInternal(buffer, pos);

    SkipSpaces(pos, buffer);
    (*pos)++; // skip ')'
    return node;
}

Node_t * BuildAsmTree(const char *buffer) {
    size_t pos = 0;
    return BuildAsmTreeInternal(buffer, &pos);
}

void CompileFile(const char * InputFile, const char * OutputFile) {
    FILE *fp = fopen(InputFile, "r");
    sassert(fp, ERR_PTR_NULL);

    size_t FileSize = get_file_size(fp);
    char *buffer = get_buffer_from_file(fp, FileSize);

    fclose(fp);

    Node_t * Tree = NULL;
    BuildAsmTree(buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        PrintHelp();
        return 0;
    }

    char output_file[MAX_STR_SIZE] = "out.txt";
    if (argc == 2)
        printf(MAGENTA "you did not type output file.\n"
                       "Compiling will be proceeded to <out.txt>\n" WHITE);
    else if (argc == 3)
        strcpy(output_file, argv[2]);
    
    CompileFile(argv[1], output_file);
    return 0;
}