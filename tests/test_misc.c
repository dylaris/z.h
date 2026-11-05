#define Z_IMPLEMENTATION
#include "../z.h"
#include "test.h"
#include <stddef.h>

typedef struct {
    int value;
    double member;
} Item;

int main(void)
{
    TEST_CHECK(wrapinc(7, 8) == 0);
    TEST_CHECK(wrapinc(3, 8) == 4);
    TEST_CHECK(wrapdec(0, 8) == 7);
    TEST_CHECK(wrapdec(3, 8) == 2);

    TEST_CHECK(clamp(5, 1, 4) == 4);
    TEST_CHECK(clamp(0, 1, 4) == 1);
    TEST_CHECK(clamp(3, 1, 4) == 3);

    TEST_CHECK(align_up(13, 8) == 16);
    TEST_CHECK(align_up(16, 8) == 16);
    TEST_CHECK(align_down(13, 8) == 8);
    TEST_CHECK(align_down(16, 8) == 16);

    TEST_CHECK(offset_of(Item, member) == offsetof(Item, member));

    Item item = {.value = 7, .member = 1.5};
    Item *found = container_of(&item.member, Item, member);
    TEST_CHECK(found == &item);
    TEST_CHECK(found->value == 7);

    return z_test_report("misc");
}
