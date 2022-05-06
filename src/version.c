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

char get_operator(const char *source)
{
    if ('^' == source[0]) {
        return '^';
    } else if ('~' == source[0]) {
        return '~';
    }

    return '=';
}

ext_code ext_version_parse(ext_app *app, char *original)
{
    char *duplicate = strdup(original);
    char *source    = trim(duplicate);
    char operator   = get_operator(source);

    if ('=' != operator)
        source += 1;

    source = trim(source);

    if (0 != semver_parse(source, &app->version.value)) {
        ext_log_error(app, "%s(): Invalid version: %s", __func__, original);
        free(duplicate);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    free(duplicate);

    app->version.operator= operator;

    return EXT_CODE_OK;
}
