#define Z_IMPLEMENTATION
#include "z.h"

int main(void)
{
    zlog_init(.level = ZLOG_INFO, .sink = stdout);

    zlog(ZLOG_INFO, "value = %d", 42);
    zlog(ZLOG_WARN, "this message is visible");

    zlog_fini();
    return 0;
}
