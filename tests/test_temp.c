#define Z_IMPLEMENTATION
#include "../z.h"
#include "test.h"
#include <stdint.h>

int main(void)
{
    for (size_t size = 1; size <= 31; size++) {
        z_ta_restore(0);
        void *first = z_ta_alloc(size);
        void *second = z_ta_alloc(sizeof(max_align_t));

        TEST_CHECK(first != NULL);
        TEST_CHECK(second != NULL);
        if (second) {
            TEST_CHECK((uintptr_t)second % _Alignof(max_align_t) == 0);
        }
    }

    z_ta_restore(0);
    size_t checkpoint = z_ta_save();
    void *saved = z_ta_alloc(32);
    z_ta_restore(checkpoint);
    void *reused = z_ta_alloc(32);
    TEST_CHECK(saved == reused);

    z_ta_restore(0);
    char *duplicate = z_ta_strdup("temporary");
    TEST_CHECK_STR(duplicate, "temporary");

    z_ta_restore(0);
    char *limited = z_ta_strndup("temporary", 5);
    TEST_CHECK_STR(limited, "tempo");

    z_ta_restore(0);
    char *formatted = z_ta_sprintf("%s:%d:%c", "value", 42, 'x');
    TEST_CHECK_STR(formatted, "value:42:x");

    z_ta_restore(0);
    char *truncated = z_ta_snprintf(5, "%s", "abcdef");
    TEST_CHECK_STR(truncated, "abcd");

    z_ta_restore(0);
    TEST_CHECK(z_ta_alloc(Z_TEMP_BUFFER_CAPACITY) != NULL);
    TEST_CHECK(z_ta_alloc(1) == NULL);

    z_ta_restore(Z_TEMP_BUFFER_CAPACITY + 1);
    TEST_CHECK(z_ta_alloc(1) == NULL);
    z_ta_restore(0);

    return z_test_report("temp allocator");
}
