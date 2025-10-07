#ifndef ERROR_MANAGE_H
#define ERROR_MANAGE_H

enum {MAX_SIZE = 200, MAX_ERROR_CODES = 32};
struct error_t{
    char error_info[MAX_ERROR_CODES][MAX_SIZE];
    int code;
    bool is_error;
};
static error_t error = {};

#define add_error(error_code, ...) {\
    strcpy(error.error_info[error_code], ("none", ##__VA_ARGS__));\
    error.code |= (1 << error_code);\
    error.is_error = true;\
}

void print_error(error_t err, const char * const error_text[]);

#endif // error_manage_h