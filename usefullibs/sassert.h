#ifndef SASSERT_H
#define SASSERT_H

#include "better_output.h"

#ifndef LEVEL_OF_CHECK
#define sassert(condition, ERROR_CODE, ...) {                                                   \
                        if (!(condition)){                                                      \
                            fprintf(stderr, RED "-->ERROR:" WHITE " in file \"%s\"\n"           \
                                                "             line %d\n", __FILE_NAME__, __LINE__);                  \
                            fprintf(stderr, "explanation: %s", ("none", ##__VA_ARGS__));                       \
                            putc('\n', stderr);                                                 \
                            exit(ERROR_CODE);                                                   \
                    }}
#endif // sassert

#endif // sassert_h