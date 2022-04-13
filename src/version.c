#include "../deps/strdup/strdup.h"
#include "../include/extendables.h"
#include "app.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *version)
{
    while (isspace((unsigned char)*version))
        version++;

    if (*version == 0)
        return version;

    char *end = version + strlen(version) - 1;

    while (end > version && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return version;
}

ext_code ext_version_parse(char *source, char **result)
{
    source = trim(source);

    if (!*result)
        return EXT_CODE_ALLOC_FAILED;

    /* trim(*result); */

    return EXT_CODE_OK;
}
