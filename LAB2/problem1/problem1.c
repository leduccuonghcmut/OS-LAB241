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
    key_t key = ftok(".", 'R'); 
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shmid = shmget(key, sizeof(movie), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    movie *arraymovie = (movie *)shmat(shmid, NULL, 0);
    if (arraymovie == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_movie; i++)
    {
        arraymovie->rating[i] = 0;
        arraymovie->averagemovie[i] = 0;
        arraymovie->freq[i] = 0;
    }

    int pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        int userID, movieID, rate, d;
        FILE *fileObj1 = fopen("movie-100k_1.txt", "r");
        while (fscanf(fileObj1, "%d %d %d %d", &userID, &movieID, &rate, &d) != EOF)
        {
            arraymovie->freq[movieID]++;
            arraymovie->rating[movieID] += rate;
        }
        exit(0);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        int userID, movieID, rate, d;
        FILE *fileObj2 = fopen("movie-100k_2.txt", "r");
        while (fscanf(fileObj2, "%d %d %d %d", &userID, &movieID, &rate, &d) != EOF)
        {
            arraymovie->freq[movieID]++;
            arraymovie->rating[movieID] += rate;
        }
        exit(0);
    }


    wait(NULL);
    wait(NULL);

    for (int i = 1; i < 1683; i++)
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
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}
