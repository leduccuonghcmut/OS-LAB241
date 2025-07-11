#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct pthread_seqlock {
    int readcount;
    int count;
    pthread_mutex_t readcountlock, countlock, wrlock;
} pthread_seqlock_t;
static inline void pthread_seqlock_init(pthread_seqlock_t *seqlock)
{
    seqlock->readcount = 0;
    seqlock->count = 0;
    pthread_mutex_init(&(seqlock->readcountlock), NULL);
    pthread_mutex_init(&(seqlock->countlock), NULL);
    pthread_mutex_init(&(seqlock->wrlock), NULL);
}
static inline void pthread_seqlock_destroy(pthread_seqlock_t *seqlock){
    pthread_mutex_destroy(&(seqlock->readcountlock));
    pthread_mutex_destroy(&(seqlock->countlock));
    pthread_mutex_destroy(&(seqlock->wrlock));
}
static inline void pthread_seqlock_wrlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_lock(&(seqlock->wrlock));
    pthread_mutex_lock(&(seqlock->countlock));
    seqlock->count = 1;
    pthread_mutex_unlock(&(seqlock->countlock));
}
static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_unlock(&(seqlock->wrlock));
    pthread_mutex_lock(&(seqlock->countlock));
    seqlock->count = 0;
    pthread_mutex_unlock(&(seqlock->countlock));
}
static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *seqlock)
{
    while(seqlock->count == 1) {};
    pthread_mutex_lock(&(seqlock->readcountlock));
    seqlock->readcount++;
    if (seqlock->readcount == 2) pthread_mutex_lock(&(seqlock->wrlock));
    pthread_mutex_unlock(&(seqlock->readcountlock));
    return 1;
}
static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_lock(&(seqlock->readcountlock));
    seqlock->readcount--;
    if (seqlock->readcount == 0) pthread_mutex_unlock(&(seqlock->wrlock));
    pthread_mutex_unlock(&(seqlock->readcountlock));
    return 1;
}
