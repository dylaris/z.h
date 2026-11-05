#define Z_IMPLEMENTATION
#define Z_ARRAY_INIT_CAPACITY 4
#include "z.h"

int main(void)
{
    int *xs = Z_ARRAY_INITIALIZER;

    for (int i = 0; i < 5; i++) arr_push(xs, i);
    arr_foreach(xs, int, iter) { printf("%d ", *iter); } printf("\n");

    printf("pop: %d\n", arr_pop(xs));
    printf("count: %zu, capacity: %zu\n", arrcnt(xs), arrcap(xs));

    arr_clear(xs);
    arr_free(xs);
    return 0;
}
