#include "../include/extendables.h"
#include "../src/version.c"
#include "../deps/ctest/ctest.h"


CTEST(version, invalid)
{
    ext_app app;
    char invalid[] = "Invalid";

    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_version_parse(&app, invalid));
}

CTEST(version, no_operator)
{
    ext_app app;
    char invalid[] = "1.5.0";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL(0, app.version.operator);
}


CTEST(version, no_operator_with_spaces)
{
    ext_app app;
    char invalid[] = "  1.5.0  ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL(0, app.version.operator);
}

CTEST(version, caret_operator)
{
    ext_app app;
    char invalid[] = "^ 2.5.1";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(2, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(1, app.version.value.patch);
    ASSERT_EQUAL('^', app.version.operator);
}

CTEST(version, caret_operator_with_spaces)
{
    ext_app app;
    char invalid[] = " ^   2.5.1 ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(2, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(1, app.version.value.patch);
    ASSERT_EQUAL('^', app.version.operator);
}

CTEST(version, tilde_operator)
{
    ext_app app;
    char invalid[] = "~ 3.5.0";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(3, app.version.value.major);
    ASSERT_EQUAL(5, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('~', app.version.operator);
}

CTEST(version, tilde_operator_with_spaces)
{
    ext_app app;
    char invalid[] = "    ~1.8.0   ";

    ASSERT_EQUAL(EXT_CODE_OK, ext_version_parse(&app, invalid));
    ASSERT_EQUAL(1, app.version.value.major);
    ASSERT_EQUAL(8, app.version.value.minor);
    ASSERT_EQUAL(0, app.version.value.patch);
    ASSERT_EQUAL('~', app.version.operator);
}

CTEST(version, invalid_operator)
{
    ext_app app;
    app.log_file = stdout;

    char invalid[] = "*~ 2.5.1";

    ASSERT_EQUAL(EXT_CODE_INVALID_ARGUMENT, ext_version_parse(&app, invalid));
}
