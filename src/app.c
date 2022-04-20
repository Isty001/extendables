#include "app.h"
#include "../deps/strdup/strdup.h"
#include "../deps/tinydir/tinydir.h"
#include "logger.h"
#include "plugin.h"
#include "util.h"
#include "version.h"
#include <stdbool.h>
#include <stdlib.h>

static ext_code load_from_installation_dir(ext_app *app, const char *path)
{
    tinydir_dir dir;

    if (-1 == tinydir_open(&dir, path)) {
        ext_log_error(app, "%s(): Cannot open installition dir: %s", __func__, path);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    ext_log_debug(app, "%s(): Loading plugins form installition dir: %s", __func__, path);

    ext_plugin *plugin = NULL;
    tinydir_file file;
    ext_code final_code = EXT_CODE_OK;
    ext_code load_code;

    while (dir.has_next) {
        tinydir_readfile(&dir, &file);

        if (file.is_dir && (0 != strcmp(".", file.name) && 0 != strcmp("..", file.name))) {
            load_code = ext_app_load(app, &plugin, file.path, NULL);

            if (EXT_CODE_OK != load_code) {
                final_code = load_code;
            }
        }

        tinydir_next(&dir);
    }

    tinydir_close(&dir);

    return final_code;
}

ext_code ext_app_init(ext_app **app, const ext_app_init_opts *opts)
{
    // @TODO check null opts
    /* ext_check_not_null_arg(opts); */

    if (!opts->version_requirement) {
        ext_log_error(*app, "%s(): Failed to initialize the app, no version_requirement provided", __func__);

        return EXT_CODE_INVALID_ARGUMENT;
    }

    ext_code code = EXT_CODE_OK;

    *app = malloc(sizeof(ext_app));

    (*app)->log_level       = opts && opts->log_level ? opts->log_level : EXT_LOG_LEVEL_DEBUG;
    (*app)->load            = opts && opts->load_function ? opts->load_function : NULL;
    (*app)->remove          = opts && opts->remove_function ? opts->remove_function : NULL;
    (*app)->plugin_list     = NULL;
    (*app)->version.operator= 0;
    (*app)->version.value   = (semver_t){0};
    (*app)->version.raw     = strdup(opts->version_requirement);

    if (EXT_CODE_OK != (code = ext_log_init(*app, opts ? opts->log_file : "stdout"))) {
        goto cleanup;
    }

    if (EXT_CODE_OK != (code = ext_version_parse(*app, opts->version_requirement))) {
        goto cleanup;
    }

    if (!(*app))
        return EXT_CODE_ALLOC_FAILED;

    if (opts->installation_dir) {
        ext_code install_code = load_from_installation_dir(*app, opts->installation_dir);

        if (EXT_CODE_OK != install_code) {
            ext_log_error(*app, "%s(): Unable to load plugins from installition dir: %s", __func__, opts->installation_dir);
            code = install_code;
        }
    }

    ext_log_debug(*app, "%s(): App initialized successfully", __func__);

    return code;

cleanup:
    free(*app);
    free((*app)->version.raw);

    return code;
}

ext_code ext_app_destroy(ext_app *app, const ext_app_destroy_opts *opts)
{
    ext_check_not_null_arg(app);

    ext_code code   = EXT_CODE_OK;
    bool had_errors = false;

    ext_app_remove_opts remove_opts;
    ext_app_plugin_item *tmp_item  = NULL;
    ext_app_plugin_item *curr_item = app->plugin_list;

    while (curr_item) {
        remove_opts.user_data = opts ? opts->remove_function_user_data : NULL;
        code                  = ext_plugin_destroy(app, curr_item->plugin, &remove_opts);

        if (EXT_CODE_OK != code) {
            had_errors = true;
        }

        tmp_item = curr_item->next;
        free(curr_item);
        curr_item = tmp_item;
    }

    semver_free(&app->version.value);
    free(app->version.raw);

    ext_log_debug(app, "%s(): App cleaned up successfully", __func__);
    ext_log_cleanup(app);
    free(app);

    if (had_errors) {
        return EXT_CODE_PLUGIN_FAILURE;
    }

    return EXT_CODE_OK;
}

ext_code ext_app_load(ext_app *app, ext_plugin **plugin, const char *path, const ext_app_load_opts *opts)
{
    ext_check_not_null_arg(app);
    ext_check_not_null_arg(path);

    ext_code code = ext_plugin_init(app, plugin, path, opts);

    if (EXT_CODE_OK != code) {
        return code;
    }

    ext_app_plugin_item *item = malloc(sizeof(ext_app_plugin_item));

    if (!item)
        return EXT_CODE_ALLOC_FAILED;

    item->plugin = *plugin;
    item->next   = NULL;

    if (!app->plugin_list) {
        app->plugin_list = item;
    } else {
        app->plugin_list->next = item;
    }

    return EXT_CODE_OK;
}

ext_code ext_app_call(const ext_app *app, ext_plugin_function function, const ext_app_call_opts *opts)
{
    ext_check_not_null_arg(app);
    ext_check_not_null_arg(function);

    return ext_plugin_call(app, function, opts);
}

ext_code ext_app_remove(ext_app *app, ext_plugin *plugin, const ext_app_remove_opts *opts)
{
    ext_check_not_null_arg(app);
    ext_check_not_null_arg(plugin);

    ext_app_plugin_item *prev_item = NULL;
    ext_app_plugin_item *curr_item = app->plugin_list;

    while (curr_item) {
        if (plugin == curr_item->plugin) {
            if (prev_item) {
                prev_item->next = curr_item->next;
            }

            if (curr_item == app->plugin_list) {
                app->plugin_list = curr_item->next;
            }

            free(curr_item);

            return ext_plugin_destroy(app, plugin, opts);
        }

        prev_item = curr_item;
        curr_item = curr_item->next;
    }

    return EXT_CODE_INVALID_ARGUMENT;
}
