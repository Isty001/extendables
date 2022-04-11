#include <stdlib.h>
#include "../deps/minunit/minunit.h"
#include "../deps/strdup/strdup.h"
#include "test.h"
#include <extendables/extendables.h>

typedef struct test_remove_ctx {
    char *parameter;
    const char *return_val;
} test_remove_ctx;

static ext_call_code plugin_remove(lua_State *lua, void *user_data)
{
    test_remove_ctx *remove_ctx = user_data;

    lua_getglobal(lua, "teardown");
    lua_pushstring(lua, remove_ctx->parameter);

    if (lua_pcall(lua, 1, 1, 0) != 0)
        return EXT_CALL_FAILED;

    if (!lua_isstring(lua, -1))
        return EXT_CALL_FAILED;

    remove_ctx->return_val = strdup(lua_tostring(lua, -1));
    lua_pop(lua, 1);

    return EXT_CALL_OK;
}

MU_TEST(test_basic_remove)
{
    ext_app *app = NULL;
    ext_app_init_opts opts = {
        .remove_function = plugin_remove,
        .log_file = "stdout"
    };

    mu_assert(EXT_CODE_OK == ext_app_init(&app, &opts), "Should return EXT_CODE_OK");

    ext_plugin *plugin = NULL;
    mu_assert(EXT_CODE_OK == ext_app_load(app, &plugin, "./test/fixture/plugin_remove_only", NULL), "Should return EXT_CODE_OK");

    test_remove_ctx remove_ctx = {
        .parameter = "Test",
    };

    ext_app_remove_opts remove_opts = {
        .user_data = &remove_ctx
    };

    mu_assert(EXT_CODE_OK == ext_app_remove(app, plugin, &remove_opts), "Should return EXT_CODE_OK");
    mu_assert_string_eq("Test Remove", remove_ctx.return_val);

    free((void *) remove_ctx.return_val);

    mu_assert(EXT_CODE_OK == ext_app_destroy(app, NULL), "Should return EXT_CODE_OK");
}

void basic_remove_test_suite(void)
{
    ext_test_suite_display();

    MU_RUN_TEST(test_basic_remove);
}
