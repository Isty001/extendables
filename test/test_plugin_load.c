#include <stdlib.h>
#include "../deps/ctest/ctest.h"
#include "../deps/strdup/strdup.h"
#include "../include/extendables.h"

typedef struct test_load_ctx {
    char *parameter;
    const char *return_val;
} test_load_ctx;

static ext_call_code plugin_load(lua_State *lua, void *user_data)
{
    test_load_ctx *load_ctx = user_data;

    lua_getglobal(lua, "load");
    lua_pushstring(lua, load_ctx->parameter);

    if (lua_pcall(lua, 1, 1, 0) != 0)
        return EXT_CALL_FAILED;

    if (!lua_isstring(lua, -1))
        return EXT_CALL_FAILED;

    load_ctx->return_val = strdup(lua_tostring(lua, -1));
    lua_pop(lua, 1);

    return EXT_CALL_OK;
}

CTEST(load, happy_path)
{
    ext_app *app = NULL;
    ext_app_init_opts opts = {
        .load_function = plugin_load,
        .log_file = "stdout",
    };

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_init(&app, &opts));

    test_load_ctx load_ctx = {
        .parameter = "Test",
    };

    ext_app_load_opts load_opts = {
        .user_data = &load_ctx
    };
    ext_plugin *plugin = NULL;

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", &load_opts));
    ASSERT_STR("Test Load", load_ctx.return_val);

    free((void *) load_ctx.return_val);

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_destroy(app, NULL));
}
