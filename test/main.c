#include "../deps/minunit/minunit.h"

void basic_setup_test_suite(void);

int main(void)
{
    MU_RUN_SUITE(basic_setup_test_suite);

    MU_REPORT();
}
