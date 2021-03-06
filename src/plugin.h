#ifndef LIB_EXTENDABLES_PLUGIN_H
#define LIB_EXTENDABLES_PLUGIN_H

#include "../deps/semver/semver.h"
#include "../include/extendables.h"
#include <lualib.h>

struct ext_plugin {
    lua_State *lua;
    const char *path;
    struct {
        char *raw;
        semver_t value;
    } api_version;
    toml_table_t *config;
};

ext_code ext_plugin_init(ext_app *app, ext_plugin **plugin_ref, const char *path, const ext_app_load_opts *opts);

ext_code ext_plugin_call(const ext_app *app, ext_plugin_function function, const ext_app_call_opts *opts);

ext_code ext_plugin_destroy(const ext_app *app, ext_plugin *plugin, const ext_app_remove_opts *opts);

#endif
