#ifndef LIB_EXTENDABLES_APP_H
#define LIB_EXTENDABLES_APP_H

#include "logger.h"
#include "plugin.h"
#include <lualib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct ext_app_plugin_item ext_app_plugin_item;

struct ext_app_plugin_item {
    ext_plugin *plugin;
    ext_app_plugin_item *next;
};

struct ext_app {
    FILE *log_file;
    ext_log_level log_level;
    ext_plugin_function load;
    ext_plugin_function remove;
    ext_app_plugin_item *plugin_list;
};

#endif
