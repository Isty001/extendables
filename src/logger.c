#include "logger.h"
#include "app.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define write_logv(log_file, format, type_str)                                                                                                                                                         \
    va_list args;                                                                                                                                                                                      \
    va_start(args, format);                                                                                                                                                                            \
    time_t rawtime;                                                                                                                                                                                    \
    struct tm *timeinfo;                                                                                                                                                                               \
                                                                                                                                                                                                       \
    time(&rawtime);                                                                                                                                                                                    \
    timeinfo = localtime(&rawtime);                                                                                                                                                                    \
    char buff[128];                                                                                                                                                                                    \
    strftime(buff, 128, "%Y:%m:%d %H:%M:%S", timeinfo);                                                                                                                                                \
                                                                                                                                                                                                       \
    fprintf(log_file, " %s - %s - ", type_str, buff);                                                                                                                                                  \
    vfprintf(log_file, format, args);                                                                                                                                                                  \
    fprintf(log_file, "\n");                                                                                                                                                                           \
    va_end(args);

void ext_log_debug(const ext_app *app, const char *format, ...)
{
    if (app->log_file && app->log_level >= EXT_LOG_LEVEL_DEBUG) {
        write_logv(app->log_file, format, "[EXT] - DEBUG");
    }
}

void ext_log_info(const ext_app *app, const char *format, ...)
{
    if (app->log_file && app->log_level >= EXT_LOG_LEVEL_INFO) {
        write_logv(app->log_file, format, "[EXT] - INFO");
    }
}

void ext_log_error(const ext_app *app, const char *format, ...)
{
    if (app->log_file && app->log_level >= EXT_LOG_LEVEL_ERROR) {
        write_logv(app->log_file, format, "[EXT] - ERROR");
    }
}

ext_code ext_log_init(ext_app *app, const char *path)
{
    if (NULL == path) {
        app->log_file = NULL;

        return EXT_CODE_OK;
    }

    if (0 == strcmp(path, "stdout")) {
        app->log_file = stdout;
    } else {
        app->log_file = fopen(path, "a+");

        if (!app->log_file)
            return EXT_CODE_NOT_FOUND;
    }

    if (!app->log_file)
        return EXT_CODE_ALLOC_FAILED;

    return EXT_CODE_OK;
}

ext_code ext_log_cleanup(ext_app *app)
{
    if (!app->log_file) {
        return EXT_CODE_OK;
    }

    if (app->log_file != stdout) {
        fclose(app->log_file);
    }

    return EXT_CODE_OK;
}
