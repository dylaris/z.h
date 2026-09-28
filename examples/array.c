#define Z_IMPLEMENTATION
#define Z_ARRAY_INIT_CAPACITY 4
#include "z.h"

int main(void)
{
    int *xs = Z_ARRAY_INITIALIZER;
    int i;
    int *iter;

    printf("====== test arr_push ======\n");
    for (i = 0; i < 100; i++) arr_push(xs, i);
    arr_foreach(xs, iter) { printf("%d ", *iter); } printf("\n");
    printf("front: %d\n", xs[0]);
    printf("back: %d\n", xs[arrcnt(xs) - 1]);
    printf("cnt: %d\n", arrcnt(xs));
    printf("cap: %d\n", arrcap(xs));

    printf("====== test arr_pop ======\n");
    i = 20;
    while (i--) printf("%d ", arr_pop(xs));
    printf("\n");
    printf("front: %d\n", xs[0]);
    printf("back: %d\n", xs[arrcnt(xs) - 1]);
    printf("cnt: %d\n", arrcnt(xs));
    printf("cap: %d\n", arrcap(xs));

    printf("====== test arr_push again ======\n");
    arr_push(xs, 999);
    arr_foreach(xs, iter) { printf("%d ", *iter); } printf("\n");
    printf("front: %d\n", xs[0]);
    printf("back: %d\n", xs[arrcnt(xs) - 1]);
    printf("cnt: %d\n", arrcnt(xs));
    printf("cap: %d\n", arrcap(xs));

    printf("====== test arr_clear ======\n");
    arr_clear(xs);
    printf("cnt: %d\n", arrcnt(xs));
    printf("cap: %d\n", arrcap(xs));

    arr_free(xs);
    return 0;
}
