#include "plugin.h"
#include "../deps/strdup/strdup.h"
#include "app.h"
#include "config.h"
#include "logger.h"
#include "util.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ENTRY_POINT "main.lua"

ext_code ext_plugin_init(ext_app *app, ext_plugin **plugin_ref, const char *path, const ext_plugin_load_opts *opts)
{
    //@TODO check if plugin->path is a directory

    ext_plugin *plugin = malloc(sizeof(ext_plugin));

    if (!plugin)
        return EXT_CODE_ALLOC_FAILED;

    plugin->api_version = NULL;
    plugin->config      = NULL;
    plugin->path        = strdup(path);

    *plugin_ref = plugin;

    size_t path_length = strlen(plugin->path) + strlen(ENTRY_POINT) + sizeof(EXT_DIRECTORY_SEPARATOR) + 1;
    char entry_point[path_length];
    snprintf(entry_point, path_length, "%s%s%s", plugin->path, EXT_DIRECTORY_SEPARATOR, ENTRY_POINT);

    ext_code code = ext_config_load(app, plugin);

    if (EXT_CODE_OK != code) {
        ext_log_error(app, "No config loaded for plugin: %s", plugin->path);

        goto cleanup;
    }

    plugin->lua = luaL_newstate();

    if (!plugin->lua) {
        code = EXT_CODE_ALLOC_FAILED;

        goto cleanup;
    }

    luaL_openlibs(plugin->lua);

    if (0 != luaL_dofile(plugin->lua, entry_point)) {
        ext_log_error(app, "ext_plugin_lua_init(): Unable to initialize plugin %s - luaL_dofile(): error loading file: %s", entry_point, lua_tostring(plugin->lua, -1));
        code = EXT_CODE_LUA_ERROR;

        goto cleanup;
    }

    if (opts && app->load) {
        code = app->load(plugin->lua, opts->user_data);
    }

    if (EXT_CODE_OK == code) {
        ext_log_debug(app, "%s(): Successfully initialized plugin: %s", __FUNCTION__, path);
    } else {
        ext_log_error(app, "%s(): Unable to initialize plugin: %s - Load function failed.", __FUNCTION__, path);

        goto cleanup;
    }

    return code;

cleanup:
    free(plugin);
    *plugin_ref = NULL;

    return code;
}

ext_code ext_plugin_destroy(const ext_app *app, ext_plugin *plugin, const ext_plugin_remove_opts *opts)
{
    ext_code code = EXT_CODE_OK;

    if (opts && app->remove) {
        code = app->remove(plugin->lua, opts->user_data);
    }

    if (EXT_CODE_OK == code) {
        ext_log_debug(app, "%s(): Successfully removed plugin: %s", __FUNCTION__, plugin->path);
    } else {
        ext_log_error(app, "%s(): Unable to remove plugin: %s - Remove function failed.", __FUNCTION__, plugin->path);
    }

    lua_close(plugin->lua);
    free((void *)plugin->path);
    free((void *)plugin->api_version);
    toml_free(plugin->config);
    free(plugin);

    return EXT_CODE_OK;
}

ext_code ext_plugin_call(const ext_app *app, ext_plugin_function function, const ext_app_call_opts *opts)
{
    ext_app_plugin_item *item = app->plugin_list;
    ext_code code             = EXT_CODE_OK;
    bool had_errors           = false;

    while (item) {
        item = item->next;

        code = function(item->plugin->lua, opts->user_data);

        if (EXT_CODE_OK != code) {
            ext_log_error(app, "%s(): Function call returned error code: %d in plugin %s", __FUNCTION__, code, item->plugin->path);
            had_errors = true;
        }
    }

    if (had_errors) {
        return EXT_CODE_PLUGIN_CALL_INCOMPLETE;
    }

    return EXT_CODE_OK;
}

ext_code ext_plugin_get_attributes(const ext_app *app, ext_plugin *plugin, toml_table_t **attributes)
{

    *attributes = toml_table_in(plugin->config, "attributes");

    if (!*attributes) {
        ext_log_debug(app, "%s(): No attributes set for plugin: %s", __FUNCTION__, plugin->path);

        return EXT_CODE_NOT_FOUND;
    }

    return EXT_CODE_OK;
}
