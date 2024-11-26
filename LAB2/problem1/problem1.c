#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_movie 1682

typedef struct
{
    int rating[1682];
    int freq[1682];
    float averagemovie[1682];
} movie;

int main()
{
    key_t key = ftok(".", 'R'); // Khóa này được sử dụng để tạo một phân vùng bộ nhớ chia sẻ (shared memory segment) bằng cách sử dụng hàm shmget()
    if (key == -1)
    {
        perror("ftok"); // Không tạo được exit failure
        exit(EXIT_FAILURE);
    }
    int shmid = shmget(key, sizeof(movie), 0666 | IPC_CREAT); // shmid :  giữ mã định danh (ID) .ID này sẽ được sử dụng để truy cập vào phân đoạn bộ nhớ
    if (shmid == -1)
    {
        perror("shmget"); // khonga tọa được exit failure
        exit(EXIT_FAILURE);
    }
    movie *arraymovie = (movie *)shmat(shmid, NULL, 0); // chương trình sau đó gán phân vùng bộ nhớ chia sẻ này vào một con trỏ arr kiểu movies bằng cách sử dụng hàm shmat()
    if (arraymovie == (void *)-1)
    {
        perror("shmat"); // không tạo được exit failure
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_movie; i++) // khởi tạo tất cả các giá trị trong mảng bằng 0
    {
        arraymovie->rating[i] = 0;
        arraymovie->averagemovie[i] = 0;
        arraymovie->freq[i] = 0;
    }

    int pid = fork(); // khỏi tạo child processes_1
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // Child process 1
    {
        int userID, movieID, rate, d;
        FILE *fileObj1 = fopen("movie-100k_1.txt", "r");                              // đọc file 1
        while (fscanf(fileObj1, "%d %d %d %d", &userID, &movieID, &rate, &d) != EOF) // đọc dữ liệu được định dạng từ một tệp vào các biến
        {
            arraymovie->freq[movieID]++;         // Cùng movieID thì tần suất ++
            arraymovie->rating[movieID] += rate; // Rate của cùng movieID thì ++
        }
        exit(0);
    }

    pid = fork(); // khỏi tạo child processes_2
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // Child process 2
    {
        int userID, movieID, rate, d;
        FILE *fileObj2 = fopen("movie-100k_2.txt", "r");                              // đọc file 2
        while (fscanf(fileObj2, "%d %d %d %d", &userID, &movieID, &rate, &d) != EOF) // đọc dữ liệu được định dạng từ một tệp vào các biến
        {
            arraymovie->freq[movieID]++;
            arraymovie->rating[movieID] += rate;
        }
        exit(0);
    }

    // Parent process đợi 2 process kết thúc
    wait(NULL);
    wait(NULL);

    for (int i = 1; i < 1683; i++) // tính trung bình ở process cha
    {
        if (arraymovie->freq[i] != 0)
        {
            arraymovie->averagemovie[i] = arraymovie->rating[i] * 1.0 / arraymovie->freq[i];
        }
        else
        {
            arraymovie->averagemovie[i] = 0;
        }
    }
    int abc;
    printf("Enter the movieID: ");
    scanf("%d", &abc);
    if (abc >= 1 && abc <= 1682)
    {
        if (arraymovie->freq[abc] != 0)
        {
            printf("The average ratings of movieID_%d: %f\n", abc, arraymovie->averagemovie[abc]);
        }
        else
        {
            printf("No ratings of movieID_%d\n", abc);
        }
    }
    else
    {
        printf("Invalid\n");
    }
    shmdt(arraymovie);
    shmctl(shmid, IPC_RMID, 0); // chương trình gỡ bỏ phân vùng bộ nhớ chia sẻ

    return 0;
}
