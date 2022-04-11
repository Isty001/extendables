#include <stdlib.h>
#include "../deps/minunit/minunit.h"
#include "../deps/strdup/strdup.h"
#include "test.h"
#include <extendables/extendables.h>

typedef struct test_load_ctx {
    char *parameter;
    const char *return_val;
} test_load_ctx;

static ext_call_code plugin_load(lua_State *lua, void *user_data)
{
    test_load_ctx *load_ctx = user_data;

    lua_getglobal(lua, "setup");
    lua_pushstring(lua, load_ctx->parameter);

    if (lua_pcall(lua, 1, 1, 0) != 0)
        return EXT_CALL_FAILED;

    if (!lua_isstring(lua, -1))
        return EXT_CALL_FAILED;

    load_ctx->return_val = strdup(lua_tostring(lua, -1));
    lua_pop(lua, 1);

    return EXT_CALL_OK;
}

MU_TEST(test_basic_setup)
{
    ext_app *app = NULL;
    ext_app_init_opts opts = {
        .load_function = plugin_load,
        .log_file = "stdout",
    };

    mu_assert(EXT_CODE_OK == ext_app_init(&app, &opts), "Should return EXT_CODE_OK");

    test_load_ctx load_ctx = {
        .parameter = "Test",
    };

    ext_app_load_opts load_opts = {
        .user_data = &load_ctx
    };
    ext_plugin *plugin = NULL;

    mu_assert(EXT_CODE_OK == ext_app_load(app, &plugin, "./test/fixture/plugin_setup_only", &load_opts), "Should return EXT_CODE_OK");
    mu_assert_string_eq("Test Load", load_ctx.return_val);

    free((void *) load_ctx.return_val);

    mu_assert(EXT_CODE_OK == ext_app_destroy(app, NULL), "Should return EXT_CODE_OK");
}

void basic_setup_test_suite(void)
{
    ext_test_suite_display();

    MU_RUN_TEST(test_basic_setup);
}
