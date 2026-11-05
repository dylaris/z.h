#ifndef Z_TESTS_TEST_H
#define Z_TESTS_TEST_H

#include <stdio.h>
#include <string.h>

static int z_test_failures = 0;
static int z_test_checks = 0;

#define TEST_CHECK(condition)                                                   \
    do {                                                                        \
        z_test_checks++;                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                       \
                    __FILE__, __LINE__, #condition);                            \
            z_test_failures++;                                                  \
        }                                                                       \
    } while (0)

#define TEST_CHECK_SIZE(actual, expected) \
    TEST_CHECK((size_t)(actual) == (size_t)(expected))

#define TEST_CHECK_STR(actual, expected)                                        \
    do {                                                                        \
        const char *z_test_actual_ = (actual);                                  \
        const char *z_test_expected_ = (expected);                              \
        TEST_CHECK(z_test_actual_ && z_test_expected_ &&                       \
                   strcmp(z_test_actual_, z_test_expected_) == 0);              \
    } while (0)

static int z_test_report(const char *name)
{
    if (z_test_failures) {
        printf("FAIL %s (%d/%d checks failed)\n",
               name, z_test_failures, z_test_checks);
        return 1;
    }

    printf("PASS %s (%d checks)\n", name, z_test_checks);
    return 0;
}

#endif // Z_TESTS_TEST_H
