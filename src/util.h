#ifndef LIB_UTIL_H
#define LIB_UTIL_H

#include <stdlib.h>

#ifdef WIN32
#define EXT_DIRECTORY_SEPARATOR "\\"
#else
#define EXT_DIRECTORY_SEPARATOR "/"
#endif

#define ext_check_not_null_arg(val)                                                                                                                                                                    \
    if (NULL == (val))                                                                                                                                                                                 \
        return EXT_CODE_INVALID_ARGUMENT;

#endif
