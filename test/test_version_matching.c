#include "../include/extendables.h"
#include "../deps/ctest/ctest.h"


CTEST(version_matching, no_api_version_requirement)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_app_init(&app, &opts));
}

CTEST(version_matching, matching_exact_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "2.0"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}

CTEST(version_matching, not_matching_exact_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "1.0"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}

CTEST(version_matching, matching_caret_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "^ 2.2"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}

CTEST(version_matching, not_matching_caret_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "^ 3.2"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}

CTEST(version_matching, matching_tilde_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "~ 2.0.1"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_OK, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}

CTEST(version_matching, not_matching_tilde_version)
{
    ext_app_init_opts opts = {
        .log_file = "stdout",
        .api_version_requirement = "~ 2.1.1"
    };

    ext_app *app = NULL;
    ASSERT_EQUAL(EXT_CALL_OK, ext_app_init(&app, &opts));

    ext_plugin *plugin;
    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_app_load(app, &plugin, "./test/fixture/plugin/load_only", NULL));

    ASSERT_EQUAL(EXT_CALL_OK, ext_app_destroy(app, NULL));
}
