#define Z_IMPLEMENTATION
#include "z.h"

int main(void)
{
    z_StringView text = z_sv_from_cstr("  hello  ");
    z_StringView trimmed = z_sv_trim(text);
    printf("trimmed: %.*s\n", z_svarg(trimmed));

    z_StringBuilder builder = {0};
    z_sb_append_cstr(&builder, "hello");
    z_sb_append_sv(&builder, z_sv_from_cstr(" world"));
    printf("builder: %.*s\n", z_svarg(z_sb_view(&builder)));
    z_sb_free(&builder);

    return 0;
}
