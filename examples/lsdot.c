#define Z_IMPLEMENTATION
#include "z.h"

int main(void)
{
    DIR *dp;
    struct dirent *ent;

    dp = opendir(".");
    if (!dp) return 1;

    while ((ent = readdir(dp))) {
        printf("[%s] %s\n", ent->d_type == DT_REG ? "FILE" : "DIR", ent->d_name);
    }

    closedir(dp);
    return 0;
}
