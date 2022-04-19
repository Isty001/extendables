#include "config.h"
#include "../deps/semver/semver.h"
#include "../deps/strdup/strdup.h"
#include "../include/toml.h"
#include "logger.h"
#include "util.h"
#include <string.h>

#define FILE_NAME "plugin.toml"

static FILE *open_file(const ext_app *app, const ext_plugin *plugin)
{
    size_t path_length = strlen(plugin->path) + strlen(FILE_NAME) + sizeof(EXT_DIRECTORY_SEPARATOR) + 1;

    char path[path_length];
    snprintf(path, path_length, "%s%s%s", plugin->path, EXT_DIRECTORY_SEPARATOR, FILE_NAME);

    FILE *file = fopen(path, "r");

    if (!file)
        ext_log_error(app, "%s(): Unable to open config file: %s", __func__, path);

    return file;
}

static char *get_required_string(const ext_app *app, toml_table_t *cfg, const char *key)
{
    toml_datum_t value = toml_string_in(cfg, key);

    if (!value.ok) {
        ext_log_error(app, "%s(): Missing mandatory configuration: %s", __func__, key);

        return NULL;
    }

    return value.u.s;
}

static ext_code parse_api_version(const ext_app *app, ext_plugin *plugin, toml_table_t *cfg)
{
    char *raw = get_required_string(app, cfg, "api_version");

    if (!raw)
        return EXT_CODE_INVALID_ARGUMENT;

    plugin->api_version.raw = strdup(raw);

    if (0 != semver_parse(raw, &plugin->api_version.value)) {
        ext_log_error(app, "%s(): Invalid api version: %s", __func__, raw);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    return EXT_CODE_OK;
}

ext_code ext_config_load(const ext_app *app, ext_plugin *plugin)
{
    FILE *file = open_file(app, plugin);

    if (!file)
        return EXT_CODE_NOT_FOUND;

    char toml_error[300];
    toml_table_t *cfg = toml_parse_file(file, toml_error, sizeof(toml_error));
    fclose(file);

    if (!cfg) {
        ext_log_error(app, "%s(): Cannot parse config: %s%s%s - %s", __func__, plugin->path, EXT_DIRECTORY_SEPARATOR, FILE_NAME, toml_error);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    toml_table_t *plugin_cfg = toml_table_in(cfg, "plugin");

    if (!plugin_cfg) {
        ext_log_error(app, "%s(): No [plugin] section in config: %s%s%s", __func__, plugin->path, EXT_DIRECTORY_SEPARATOR, FILE_NAME);
        toml_free(cfg);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    ext_code code = parse_api_version(app, plugin, plugin_cfg);

    if (EXT_CODE_OK != code)
        return code;

    plugin->config = cfg;

    ext_log_debug(app, "%s(): Config loaded: %s%s%s", __func__, plugin->path, EXT_DIRECTORY_SEPARATOR, FILE_NAME);

    return EXT_CODE_OK;
}
