#define Z_IMPLEMENTATION
#include "z.h"

int main(void)
{
    zlog_init(.level = ZLOG_WARN, .sink = stderr);

    zlog(ZLOG_INFO, "this is info log");
    zlog(ZLOG_WARN, "this is warn log");
    zlog(ZLOG_ERROR, "this is error log");
    zlog(ZLOG_FATAL, "this is fatal log");

    zlog_fini();
    return 0;
}
