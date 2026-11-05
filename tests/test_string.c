#include <stdlib.h>

static int realloc_calls;
static int free_calls;

static void *test_realloc(void *ptr, size_t size)
{
    realloc_calls++;
    return realloc(ptr, size);
}

static void test_free(void *ptr)
{
    free_calls++;
    free(ptr);
}

#define Z_REALLOC test_realloc
#define Z_FREE test_free
#define Z_IMPLEMENTATION
#include "../z.h"
#include "test.h"

int main(void)
{
    z_StringView null_view = z_sv_from_cstr(NULL);
    TEST_CHECK_SIZE(null_view.count, 0);
    TEST_CHECK(null_view.data == NULL);

    z_StringView text = z_sv_from_cstr(" \t hello world \n");
    z_StringView trimmed = z_sv_trim(text);
    TEST_CHECK_SIZE(trimmed.count, 11);
    TEST_CHECK(memcmp(trimmed.data, "hello world", 11) == 0);

    z_StringView part = z_sv_substr(text, 3, 5);
    TEST_CHECK_SIZE(part.count, 5);
    TEST_CHECK(memcmp(part.data, "hello", 5) == 0);

    part = z_sv_substr(text, text.count, 3);
    TEST_CHECK_SIZE(part.count, 0);
    TEST_CHECK(part.data == NULL);

    TEST_CHECK(z_sv_starts_with(trimmed, z_sv_from_cstr("hello")));
    TEST_CHECK(z_sv_ends_with(trimmed, z_sv_from_cstr("world")));
    TEST_CHECK(z_sv_equal(trimmed, z_sv_from_cstr("hello world")));
    TEST_CHECK(!z_sv_equal(trimmed, z_sv_from_cstr("hello")));

    z_StringBuilder builder = {0};
    z_sb_append_char(&builder, 'A');
    z_sb_append_cstr(&builder, "ello");
    z_sb_append_sv(&builder, z_sv_from_cstr(" world"));
    z_sb_append_parts(&builder, "!", 1);
    z_sb_append_parts(&builder, NULL, 0);
    z_sb_append_cstr(&builder, NULL);

    z_StringView built = z_sb_view(&builder);
    TEST_CHECK_SIZE(built.count, 12);
    TEST_CHECK(memcmp(built.data, "Aello world!", 12) == 0);
    TEST_CHECK(realloc_calls > 0);

    z_sb_clear(&builder);
    TEST_CHECK_SIZE(builder.count, 0);
    z_sb_append_cstr(&builder, "x");
    built = z_sb_view(&builder);
    TEST_CHECK_SIZE(built.count, 1);
    TEST_CHECK(built.data[0] == 'x');
    z_sb_free(&builder);
    TEST_CHECK(free_calls > 0);
    TEST_CHECK(builder.chars == NULL);

    return z_test_report("string");
}
