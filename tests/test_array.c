#define Z_IMPLEMENTATION
#define Z_ARRAY_INIT_CAPACITY 4
#include "../z.h"
#include "test.h"

typedef struct {
    int x;
    int y;
} Point;

int main(void)
{
    int *values = Z_ARRAY_INITIALIZER;
    int visits = 0;

    TEST_CHECK_SIZE(arrcnt(values), 0);
    TEST_CHECK_SIZE(arrcap(values), 0);
    arr_foreach(values, int, iter) {
        (void)iter;
        visits++;
    }
    TEST_CHECK_SIZE(visits, 0);

    for (int i = 0; i < 100; i++) arr_push(values, i);
    TEST_CHECK_SIZE(arrcnt(values), 100);
    TEST_CHECK(arrcap(values) >= 100);

    for (int i = 0; i < 100; i++) TEST_CHECK(values[i] == i);

    for (int i = 99; i >= 80; i--) TEST_CHECK(arr_pop(values) == i);
    TEST_CHECK_SIZE(arrcnt(values), 80);

    arr_push(values, 1000);
    TEST_CHECK_SIZE(arrcnt(values), 81);
    TEST_CHECK(values[80] == 1000);

    size_t capacity = arrcap(values);
    arr_clear(values);
    TEST_CHECK_SIZE(arrcnt(values), 0);
    TEST_CHECK_SIZE(arrcap(values), capacity);

    visits = 0;
    arr_foreach(values, int, iter) {
        (void)iter;
        visits++;
    }
    TEST_CHECK_SIZE(visits, 0);
    arr_free(values);
    TEST_CHECK(values == NULL);

    Point *points = Z_ARRAY_INITIALIZER;
    arr_push(points, ((Point){.x = 1, .y = 2}));
    arr_push(points, ((Point){.x = 3, .y = 4}));
    TEST_CHECK_SIZE(arrcnt(points), 2);

    int point_index = 0;
    arr_foreach(points, Point, iter) {
        TEST_CHECK(iter->x == point_index * 2 + 1);
        TEST_CHECK(iter->y == point_index * 2 + 2);
        point_index++;
    }
    TEST_CHECK(point_index == 2);
    arr_free(points);

    return z_test_report("array");
}
