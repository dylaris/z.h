#define Z_IMPLEMENTATION
#define Z_DEQUE_INIT_CAPACITY 4
#include "z.h"

int main(void)
{
    int *deq = Z_DEQUE_INITIALIZER;
    int *iter;
    int i;

    printf("====== test deq_push ======\n");
    for (i = 0; i < 2; i++) deq_push_back(deq, i);
    for (i = 80; i > 0; i--) deq_push_front(deq, i);
    deq_foreach(deq, iter) { printf("%d ", *iter); } printf("\n");
    printf("head: %d\n", deqhead(deq));
    printf("tail: %d\n", deqtail(deq));
    printf("cnt:  %d\n", deqcnt(deq));
    printf("cap:  %d\n", deqcap(deq));

    printf("====== test deq_pop ======\n");
    for (i = 0; i < 20; i++) { printf("%d ", deq_pop_front(deq)); } printf("\n");
    for (i = 0; i < 20; i++) { printf("%d ", deq_pop_back(deq)); } printf("\n");
    deq_foreach(deq, iter) { printf("%d ", *iter); } printf("\n");
    printf("head: %d\n", deqhead(deq));
    printf("tail: %d\n", deqtail(deq));
    printf("cnt:  %d\n", deqcnt(deq));
    printf("cap:  %d\n", deqcap(deq));

    printf("====== test deq_push again ======\n");
    for (i = 0; i < 32; i++) deq_push_back(deq, i+10000);
    for (i = 80; i > 0; i--) deq_push_front(deq, i+10000);
    deq_foreach(deq, iter) { printf("%d ", *iter); } printf("\n");
    printf("head: %d\n", deqhead(deq));
    printf("tail: %d\n", deqtail(deq));
    printf("cnt:  %d\n", deqcnt(deq));
    printf("cap:  %d\n", deqcap(deq));

    printf("====== test deq_clear ======\n");
    deq_clear(deq);
    printf("cnt: %d\n", deqcnt(deq));
    printf("cap: %d\n", deqcap(deq));

    deq_free(deq);
    return 0;
}
