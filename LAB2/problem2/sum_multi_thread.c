#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct 
{
    int a;         
    int b;         
    long long sum; 
} class_sum;

void *sum_thread(void *arg)
{
    class_sum *temp = (class_sum *)arg; 
    temp->sum = 0;                      
    for (int i = temp->a; i <= temp->b; i++)
    {
        temp->sum += i; 
    }
    printf("ThreadID: %lu, Sum: %lld\n", pthread_self(), temp->sum); 
    return NULL;                                                    
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("./sum_multi_thread <num_threads> <n>\n"); 
        return 1;
    }
    if (atoi(argv[1]) < 1) 
    {
        printf("MATH ERROR OR SYNTAX\n");
        return 0;
    }

    int n = atoi(argv[1]);                                            
    int m = atoi(argv[2]);                                            
    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));  
    class_sum *arraysum = (class_sum *)malloc(n * sizeof(class_sum)); 

    int len = m / n; 
    for (int i = 0; i < n; i++)
    {
        arraysum[i].a = i * len + 1; 
        arraysum[i].sum = 0;         

        if (i == n - 1)
        {
            arraysum[i].b = m;
        }
        else
        {
            arraysum[i].b = arraysum[i].a + len - 1;
        }

        pthread_create(&threads[i], NULL, sum_thread, &arraysum[i]); 
    }

    long long sum = 0;
    for (int i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL); 
        sum += arraysum[i].sum;         
    }

    printf("Tong cua day so tu 1 den %d la: %lld\n", m, sum);

    free(threads);  
    free(arraysum); 

    return 0;
}
