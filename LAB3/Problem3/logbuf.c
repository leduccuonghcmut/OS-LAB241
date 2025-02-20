#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h> 

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];
sem_t sem_1,sem_2;

int count;
void flushlog();

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;

   usleep(20);
   sem_wait(&sem_2); // check if buffer's slot is available
   sprintf(str, "%d", id);
   sem_wait(&sem_1); // lock sem_1
   strcpy(logbuf[count], str);
   count = (count >= MAX_BUFFER_SLOT)? count :(count + 1); /* Only increase count to size MAX_BUFFER_SLOT*/
   sem_post(&sem_1); // unlock sem_1
   //printf("wrlog(): %d \n", id);

   return 0;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];

   //printf("flushlog()\n");
   sprintf(nullval, "%d", -1);
   for (i = 0; i < count; i++)
   {
      printf("Slot  %i: %s\n", i, logbuf[i]);
      strcpy(logbuf[i], nullval);
   }

   fflush(stdout);

   // reset buffer
   sem_wait(&sem_1); // lock sem_1 
   count = 0;
   sem_post(&sem_1); // unlock sem_1
   int value;
   
   // reset sem_2
   sem_getvalue(&sem_2, &value); 
   for(int i=1;i<=6-value;i++) sem_post(&sem_2);
   
   return;

}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];
   sem_init(&sem_1,0,1);
   sem_init(&sem_2,0,MAX_BUFFER_SLOT);

   struct _args args;
   args.interval = 500e3;
   /*500 msec ~ 500 * 1000 usec */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   sleep(5);

   return 0;
}

