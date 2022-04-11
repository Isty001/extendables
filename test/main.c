#include "../deps/minunit/minunit.h"

void test_basic_load(void);
void test_basic_remove(void);

int main(void)
{
    MU_RUN_TEST(test_basic_load);
    MU_RUN_TEST(test_basic_remove);

    MU_REPORT();
}
