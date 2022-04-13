#include <stdlib.h>
#include "../deps/ctest/ctest.h"
#include "../deps/strdup/strdup.h"
#include <extendables/extendables.h>

typedef struct test_call_ctx {
    char *result;
} test_call_ctx;

static ext_call_code collect_animals(lua_State *lua, void *user_data)
{
    test_call_ctx *call_ctx = user_data;

    lua_getglobal(lua, "add_animal");
    lua_pushstring(lua, call_ctx->result);

    if (lua_pcall(lua, 1, 1, 0) != 0)
        return EXT_CALL_FAILED;

    if (!lua_isstring(lua, -1))
        return EXT_CALL_FAILED;

    free(call_ctx->result);

    call_ctx->result = strdup(lua_tostring(lua, -1));
    lua_pop(lua, 1);

    return EXT_CALL_OK;
}

CTEST(test_call, basic)
{
    ext_app *app = NULL;
    ext_app_init_opts opts = {
        .log_file = "stdout"
    };

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin = NULL;
    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/add_animal_1", NULL));
    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/add_animal_2", NULL));

    test_call_ctx call_ctx = {
        .result = strdup("Animals: "),
    };

    ext_app_call_opts call_opts = {
        .user_data = &call_ctx
    };

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_call(app, collect_animals, &call_opts));
    ASSERT_STR("Animals: Dog, Cat", call_ctx.result);

    free((void *)call_ctx.result);

    ASSERT_EQUAL(EXT_CODE_OK, ext_app_destroy(app, NULL));
}
