#include "app.h"
#include "logger.h"
#include "plugin.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>

ext_code ext_app_init(ext_app **app, const ext_app_init_opts *opts)
{
    ext_code code = EXT_CODE_OK;

    *app = malloc(sizeof(ext_app));

    (*app)->log_level   = opts && opts->log_level ? opts->log_level : EXT_LOG_LEVEL_DEBUG;
    (*app)->load        = opts && opts->load_function ? opts->load_function : NULL;
    (*app)->remove      = opts && opts->remove_function ? opts->remove_function : NULL;
    (*app)->plugin_list = NULL;

    code = ext_log_init(*app, opts ? opts->log_file : "stdout");

    if (EXT_CODE_OK != code) {
        free(*app);
        return code;
    }

    if (!(*app))
        return EXT_CODE_ALLOC_FAILED;

    ext_log_debug(*app, "%s(): App initialized successfully", __func__);

    return code;
}

ext_code ext_app_destroy(ext_app *app, const ext_app_destroy_opts *opts)
{
    ext_check_not_null_arg(app);

    ext_code code   = EXT_CODE_OK;
    bool had_errors = false;

    ext_plugin_remove_opts remove_opts;
    ext_app_plugin_item *tmp_item  = NULL;
    ext_app_plugin_item *curr_item = app->plugin_list;

    while (curr_item) {
        remove_opts.user_data = opts ? opts->remove_function_user_data : NULL;
        code = ext_plugin_destroy(app, curr_item->plugin, &remove_opts);

        if (EXT_CODE_OK != code) {
            had_errors = true;
        }

        tmp_item = curr_item->next;
        free(curr_item);
        curr_item = tmp_item;
    }

    ext_log_debug(app, "%s(): App cleaned up successfully", __func__);

    ext_log_cleanup(app);
    free(app);

    if (had_errors) {
        return EXT_CODE_PLUGIN_CALL_INCOMPLETE;
    }

    return EXT_CODE_OK;
}

ext_code ext_app_load(ext_app *app, ext_plugin **plugin, const char *path, const ext_plugin_load_opts *opts)
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

ext_code ext_app_remove(ext_app *app, ext_plugin *plugin, const ext_plugin_remove_opts *opts)
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

            free(curr_item);

            return ext_plugin_destroy(app, plugin, opts);
        }

        prev_item = curr_item;
        curr_item = curr_item->next;
    }

    return EXT_CODE_INVALID_ARGUMENT;
}
