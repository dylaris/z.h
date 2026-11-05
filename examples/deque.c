#define Z_IMPLEMENTATION
#define Z_DEQUE_INIT_CAPACITY 4
#include "z.h"

int main(void)
{
    int *deq = Z_DEQUE_INITIALIZER;

    deq_push_back(deq, 1);
    deq_push_back(deq, 2);
    deq_push_front(deq, 0);
    deq_foreach(deq, int, iter) { printf("%d ", *iter); } printf("\n");

    printf("head: %d, tail: %d\n", deqhead(deq), deqtail(deq));
    int front = deq_pop_front(deq);
    int back = deq_pop_back(deq);
    printf("pop front: %d, pop back: %d\n", front, back);

    deq_free(deq);
    return 0;
}
