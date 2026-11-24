#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    char dir_path[256];
    int K;
    if (scanf("%255s %d", dir_path, &K) != 2) {
        return 0;
    }

    if (K <= 0) {
        printf("-1\n");
        return 0;
    }

    char sub_path[512];
    for (int i = 1; i <= K; i++) {
        snprintf(sub_path, sizeof(sub_path), "%s/sub_%d", dir_path, i);
        mkdir(sub_path, 0755); // Bỏ qua giá trị trả về vì thư mục có thể đã tồn tại sẵn
    }

    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("-1\n");
        return 0;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        count++;
    }

    closedir(dir);
    printf("%d\n", count);
    return 0;
}
