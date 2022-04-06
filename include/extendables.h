#ifndef LIB_EXTENDABLES_H
#define LIB_EXTENDABLES_H

#include "toml.h"
#include <lua.h>
#include <stdint.h>

typedef enum { EXT_CODE_OK, EXT_CODE_ALLOC_FAILED, EXT_CODE_LUA_ERROR, EXT_CODE_INVALID_ARGUMENT, EXT_CODE_PLUGIN_CALL_INCOMPLETE, EXT_CODE_NOT_FOUND } ext_code;

typedef enum {
    EXT_LOG_LEVEL_DEBUG = 500,
    EXT_LOG_LEVEL_INFO  = 300,
    EXT_LOG_LEVEL_ERROR = 100,
} ext_log_level;

typedef ext_code (*ext_plugin_function)(lua_State *lua, void *user_data);

typedef struct {
    ext_log_level log_level;
    char *log_file;
    ext_plugin_function load_functon;
    ext_plugin_function remove_function;
} ext_app_init_opts;

typedef struct ext_app ext_app;

ext_code ext_app_init(ext_app **app, const ext_app_init_opts *opts);

ext_code ext_app_destroy(ext_app *app);

typedef struct {
    void *user_data;
} ext_app_call_opts;

ext_code ext_app_call(const ext_app *app, ext_plugin_function function, const ext_app_call_opts *opts);

// PLUGIN
typedef struct ext_plugin ext_plugin;

typedef struct {
    void *user_data;
} ext_plugin_load_opts;

ext_code ext_app_load(ext_app *app, ext_plugin **plugin, const char *path, const ext_plugin_load_opts *opts);

ext_code ext_plugin_get_attributes(const ext_app *app, ext_plugin *plugin, toml_table_t **attributes);

typedef struct {
    void *user_data;
} ext_plugin_remove_opts;

ext_code ext_app_remove(ext_app *app, ext_plugin *plugin, const ext_plugin_remove_opts *opts);

#endif
