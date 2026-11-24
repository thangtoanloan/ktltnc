#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct Record {
    int32_t id;
    int32_t score;
};

int main() {
    char file_path[256];
    int q;
    if (scanf("%255s %d", file_path, &q) != 2) {
        return 0;
    }

    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    // Kiểm tra kích thước tệp có chia hết cho 8 hay không (8 = sizeof(struct Record))
    if (st.st_size % (off_t)sizeof(struct Record) != 0) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    long long N = st.st_size / sizeof(struct Record);

    // Xử lý q lệnh cập nhật
    for (int i = 0; i < q; i++) {
        long long idx, delta;
        if (scanf("%lld %lld", &idx, &delta) != 2) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        // Bỏ qua an toàn nếu idx nằm ngoài phạm vi [0, N - 1]
        if (idx < 0 || idx >= N) {
            continue;
        }

        off_t offset = (off_t)idx * sizeof(struct Record);
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        struct Record rec;
        if (read(fd, &rec, sizeof(struct Record)) != (ssize_t)sizeof(struct Record)) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        rec.score += (int32_t)delta;

        // Định vị lại con trỏ tệp về vị trí bản ghi để ghi đè
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        if (write(fd, &rec, sizeof(struct Record)) != (ssize_t)sizeof(struct Record)) {
            close(fd);
            printf("-1\n");
            return 0;
        }
    }

    // Đưa con trỏ tệp về đầu tệp để đọc lại toàn bộ N bản ghi và tính tổng score
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    long long total_score = 0;
    struct Record rec;
    while (read(fd, &rec, sizeof(struct Record)) == (ssize_t)sizeof(struct Record)) {
        total_score += rec.score;
    }

    close(fd);
    printf("%lld\n", total_score);
    return 0;
}
