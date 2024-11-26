#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>      //mmap()
#include <sys/stat.h>      //ftruncate() và các thao tác file
#include <fcntl.h>         //open()
#include <unistd.h>        //fork(),close(),...
#include <string.h>        //strcpy()
#include <sys/wait.h>      //wait()
#define FILENAME "shared_file.dat"

int main() {
    int fd;
    char* shared_data;
    struct stat st;
    off_t size;
    
    // Mở file với quyền đọc/ghi và tạo file mới nếu chưa tồn tại
    fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    size = 1024;  /// Kích thước vùng nhớ chia sẻ (1KB)
    
    // Đặt kích thước file bằng `ftruncate` để đảm bảo đúng kích thước
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Tạo vùng nhớ chia sẻ với mmap
    shared_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Ghi dữ liệu vào vùng nhớ chia sẻ
    strcpy(shared_data, "Hello, shared memory!");
    
    // Tạo một tiến trình con bằng fork()
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        printf("Child process: %s\n", shared_data); // Đọc dữ liệu từ vùng nhớ chia sẻ
    } else {
        wait(NULL);
    }

    // Hủy ánh xạ vùng nhớ chia sẻ
    if (munmap(shared_data, size) == -1) {
        perror("munmap");
        exit(1);
    }
    close(fd);

    return 0;
}