#ifndef LIB_EXTENDABLES_LOGGER_H
#define LIB_EXTENDABLES_LOGGER_H

#include "../include/extendables.h"

ext_code ext_log_init(ext_app *app, const char *path);

ext_code ext_log_cleanup(ext_app *app);

void ext_log_debug(const ext_app *app, const char *format, ...);

void ext_log_info(const ext_app *app, const char *format, ...);

void ext_log_error(const ext_app *app, const char *format, ...);

#endif
