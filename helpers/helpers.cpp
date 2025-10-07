#include "../stack/stack.h"
#include "helpers.h"
#include "../usefullibs/error_manage.h"

bool is_file_exists(const char * file_name) {
    sassert(file_name, ERR_PTR_NULL);

    FILE * fp = fopen(file_name, "r");
    if (fp == NULL) {
        add_error(ERR_FILE_DOES_NOT_EXIST);
        return 0;
    }
    fclose(fp);
    return 1;
}

bool is_same(double a, double b) {
    return abs(a - b) < FLT_ERR;
}

void nullify_anything_extra(char * buffer, size_t file_size, size_t actually_read) {
    sassert(buffer != NULL, ERR_PTR_NULL);

    while (actually_read < file_size) {
        buffer[++actually_read] = '\0';
    }
}

char * get_buffer_from_file(char * user_file_compile) {
    sassert(user_file_compile, ERR_PTR_NULL);

    FILE * fp = fopen(user_file_compile, "r");
    sassert(fp, ERR_PTR_NULL);

    size_t file_size = get_file_size(fp);
    char * compile_buffer = (char *) calloc(file_size + 1, sizeof(char));
    sassert(compile_buffer, ERR_PTR_NULL);

    size_t actually_read = fread(compile_buffer, sizeof(char), file_size, fp);
    nullify_anything_extra(compile_buffer, file_size, actually_read);

    return compile_buffer;
}

size_t get_file_size(FILE * fp) {
    sassert(fp, ERR_PTR_NULL);

    fseek(fp, 0, SEEK_END);
    size_t file_size = (size_t) ftell(fp);
    rewind(fp);

    return file_size;
}

bool is_in_array(const char * command, size_t size) {
    sassert(command,      ERR_PTR_NULL);
    sassert(all_commands, ERR_PTR_NULL);

    for (size_t i = 0; i < size; i++) {
        if (strcmp(command, all_commands[i]) == 0)
            return true;
    }

    return false;
}