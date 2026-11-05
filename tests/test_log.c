#define Z_IMPLEMENTATION
#include "../z.h"
#include "test.h"

int main(void)
{
    const char *filename = "test_z_log.tmp";
    char buffer[1024] = {0};

    remove(filename);
    zlog_init(.level = ZLOG_WARN, .filepath = filename);
    zlog(ZLOG_INFO, "hidden");
    zlog(ZLOG_WARN, "visible %d", 42);
    zlog(ZLOG_ERROR, "error text");
    zlog_fini();
    zlog_fini();

    FILE *file = fopen(filename, "r");
    TEST_CHECK(file != NULL);
    if (file) {
        size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, file);
        buffer[bytes] = '\0';
        fclose(file);
    }

    TEST_CHECK(strstr(buffer, "[WARN]") != NULL);
    TEST_CHECK(strstr(buffer, "visible 42") != NULL);
    TEST_CHECK(strstr(buffer, "[ERROR]") != NULL);
    TEST_CHECK(strstr(buffer, "[INFO]") == NULL);
    TEST_CHECK(strstr(buffer, "hidden") == NULL);

    remove(filename);
    return z_test_report("log");
}
