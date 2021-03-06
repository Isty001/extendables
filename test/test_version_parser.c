#include "../src/version.h"
#include "../include/extendables.h"
#include "../deps/ctest/ctest.h"


static ext_app create_app(void)
{
    ext_app app;
    app.log_file = stdout;
    app.log_level = EXT_LOG_LEVEL_DEBUG;
    app.version.value = (semver_t){0};
    app.version.operator = 0;

    return app;
}

CTEST(version_parser, invalid)
{
    ext_app app = create_app();
    char invalid[] = "Invalid";

    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_version_parse(&app, invalid));
}

CTEST(version_parser, default_operator)
{
    ext_app app = create_app();
    char invalid[] = "1.5.0";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('=', app.version.operator);
}


CTEST(version_parser, default_operator_with_spaces)
{
    ext_app app = create_app();
    char invalid[] = "  1.5.0  ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('=', app.version.operator);
}

CTEST(version_parser, caret_operator)
{
    ext_app app = create_app();
    char invalid[] = "^ 2.5.1";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(2, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(1, app.version.value.patch);
    ASSERT_EQUAL('^', app.version.operator);
}

CTEST(version_parser, caret_operator_with_spaces)
{
    ext_app app = create_app();
    char invalid[] = " ^   2.5.1 ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(2, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(1, app.version.value.patch);
    ASSERT_EQUAL('^', app.version.operator);
}

CTEST(version_parser, tilde_operator)
{
    ext_app app = create_app();
    char invalid[] = "~ 3.5.0";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(3, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('~', app.version.operator);
}

CTEST(version_parser, tilde_operator_with_spaces)
{
    ext_app app = create_app();
    char invalid[] = "    ~1.8.0   ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(8, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('~', app.version.operator);
}

CTEST(version_parser, invalid_operator)
{
    ext_app app = create_app();
    app.log_file = stdout;

    char invalid[] = "*~ 2.5.1";

    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_version_parse(&app, invalid));

    ASSERT_EQUAL(0, app.version.value.major);
    ASSERT_EQUAL(0, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL(0, app.version.operator);
}
