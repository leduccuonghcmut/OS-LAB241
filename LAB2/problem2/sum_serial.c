#include <stdio.h>
#include <stdlib.h>

long long sumstring(int n)
{
    long long temp = (long long)n * (n + 1) / 2; 
    return temp;
}

int main(int argc, char *argv[]) 
{
    if (argc != 2)
    {
        
        return 1;
    }
    if (atoi(argv[1]) < 1) 
    {
        printf("MATH ERROR OR SYNTAX\n"); 
        return 0;
    }
    int num = atoi(argv[1]);          
    printf("%lld\n", sumstring(num)); 
    return 0;
}