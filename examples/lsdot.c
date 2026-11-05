#define Z_IMPLEMENTATION
#include "z.h"

int main(void)
{
    DIR *dp = opendir(".");
    struct dirent *ent;

    if (!dp) return 1;

    while ((ent = readdir(dp))) {
        printf("%s %s\n", ent->d_type == DT_DIR ? "dir " : "file", ent->d_name);
    }

    closedir(dp);
    return 0;
}
