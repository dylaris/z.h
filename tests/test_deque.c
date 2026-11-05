#define Z_IMPLEMENTATION
#define Z_DEQUE_INIT_CAPACITY 4
#include "../z.h"
#include "test.h"

static void check_values(int *deq, const int *expected, size_t count)
{
    size_t index = 0;

    TEST_CHECK_SIZE(deqcnt(deq), count);
    deq_foreach(deq, int, iter) {
        TEST_CHECK(index < count);
        if (index < count) TEST_CHECK(*iter == expected[index]);
        index++;
    }
    TEST_CHECK_SIZE(index, count);
}

static void check_model(int *deq, const int *model, size_t head,
                        size_t count, size_t capacity)
{
    size_t index = 0;

    TEST_CHECK_SIZE(deqcnt(deq), count);
    deq_foreach(deq, int, iter) {
        TEST_CHECK(index < count);
        if (index < count) {
            TEST_CHECK(*iter == model[(head + index) % capacity]);
        }
        index++;
    }
    TEST_CHECK_SIZE(index, count);
}

int main(void)
{
    int *deq = Z_DEQUE_INITIALIZER;
    int visits = 0;

    TEST_CHECK_SIZE(deqcnt(deq), 0);
    deq_foreach(deq, int, iter) {
        (void)iter;
        visits++;
    }
    TEST_CHECK_SIZE(visits, 0);

    for (int value = 1; value <= 4; value++) deq_push_back(deq, value);
    {
        const int expected[] = {1, 2, 3, 4};
        check_values(deq, expected, 4);
    }

    TEST_CHECK(deq_pop_front(deq) == 1);
    deq_push_back(deq, 5);
    {
        const int expected[] = {2, 3, 4, 5};
        check_values(deq, expected, 4);
    }
    TEST_CHECK(deqhead(deq) == 2);
    TEST_CHECK(deqtail(deq) == 5);

    TEST_CHECK(deq_pop_back(deq) == 5);
    deq_push_front(deq, 9);
    {
        const int expected[] = {9, 2, 3, 4};
        check_values(deq, expected, 4);
    }

    deq_clear(deq);
    TEST_CHECK_SIZE(deqcnt(deq), 0);
    visits = 0;
    deq_foreach(deq, int, iter) {
        (void)iter;
        visits++;
    }
    TEST_CHECK_SIZE(visits, 0);

    for (int value = 1; value <= 4; value++) deq_push_back(deq, value);
    TEST_CHECK(deq_pop_front(deq) == 1);
    TEST_CHECK(deq_pop_front(deq) == 2);
    TEST_CHECK(deq_pop_front(deq) == 3);
    deq_push_back(deq, 9);
    deq_push_back(deq, 10);
    deq_push_back(deq, 11);
    deq_push_back(deq, 12);
    {
        const int expected[] = {4, 9, 10, 11, 12};
        check_values(deq, expected, 5);
    }
    deq_free(deq);
    TEST_CHECK(deq == NULL);

    enum { MODEL_CAPACITY = 4096, STEPS = 10000 };
    int model[MODEL_CAPACITY];
    size_t model_head = 0;
    size_t model_count = 0;
    unsigned state = 123456789u;
    deq = Z_DEQUE_INITIALIZER;

    for (int step = 0; step < STEPS; step++) {
        state = state * 1664525u + 1013904223u;
        int operation = (int)(state % 5u);
        int value = (int)state;

        if (model_count == 0) {
            operation = 2;
        } else if (model_count == MODEL_CAPACITY) {
            operation = 3;
        }

        if (operation == 0) {
            deq_push_front(deq, value);
            model_head = (model_head + MODEL_CAPACITY - 1) % MODEL_CAPACITY;
            model[model_head] = value;
            model_count++;
        } else if (operation <= 2) {
            deq_push_back(deq, value);
            model[(model_head + model_count) % MODEL_CAPACITY] = value;
            model_count++;
        } else if (operation == 3) {
            int expected = model[model_head];
            TEST_CHECK(deq_pop_front(deq) == expected);
            model_head = (model_head + 1) % MODEL_CAPACITY;
            model_count--;
        } else {
            int expected = model[(model_head + model_count - 1) % MODEL_CAPACITY];
            TEST_CHECK(deq_pop_back(deq) == expected);
            model_count--;
        }

        TEST_CHECK_SIZE(deqcnt(deq), model_count);
        if (step % 97 == 0) {
            check_model(deq, model, model_head, model_count, MODEL_CAPACITY);
        }
    }

    check_model(deq, model, model_head, model_count, MODEL_CAPACITY);
    deq_free(deq);

    return z_test_report("deque");
}
