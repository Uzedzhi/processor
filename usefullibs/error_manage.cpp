#include <string.h>
#include <stdio.h>
#include "../usefullibs/error_manage.h"
#include "../usefullibs/better_output.h"

void print_error(error_t err, const char * const error_text[]) {
    if (err.is_error == true)
        printf(RED "all errors:\n" WHITE);
    size_t count = 0;
    while (err.code != 0) {
        if ((err.code & 1) == 1) {
            if (strlen(err.error_info[count]) != 1)
                printf("%s -> %s\n", error_text[count], err.error_info[count]);
            else
                printf("%s\n", error_text[count]);
        }
        err.code >>= 1;
        count++;
    }
}