#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    char path[512];
    if (scanf("%511s", path) != 1) return 0;

    struct stat st;
    if (stat(path, &st) < 0) {
        printf("-1\n");
        return 0;
    }

    const char *type;
    if (S_ISREG(st.st_mode)) {
        type = "REG";
    } else if (S_ISDIR(st.st_mode)) {
        type = "DIR";
    } else {
        type = "OTHER";
    }

    printf("%s %lld %03o\n", type, (long long)st.st_size,
           (unsigned int)(st.st_mode & 0777));
    return 0;
}
