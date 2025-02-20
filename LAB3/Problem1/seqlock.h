#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct pthread_seqlock {
    int readcount; // Number of readers holding the lock
    int count; // Flag to indicate if there is a writer holding the lock
    pthread_mutex_t readcountlock, countlock, wrlock; // Mutex locks for readcount, count, and write lock
} pthread_seqlock_t;
static inline void pthread_seqlock_init(pthread_seqlock_t *seqlock)
{
    seqlock->readcount = 0; // Initialize the number of readers to 0
    seqlock->count = 0; // Initialize the flag to indicate no writers are holding the lock
    pthread_mutex_init(&(seqlock->readcountlock), NULL); // Initialize the mutex for readcount
    pthread_mutex_init(&(seqlock->countlock), NULL); // Initialize the mutex for count
    pthread_mutex_init(&(seqlock->wrlock), NULL); // Initialize the mutex for write lock
}
static inline void pthread_seqlock_destroy(pthread_seqlock_t *seqlock){
    pthread_mutex_destroy(&(seqlock->readcountlock)); // Destroy the mutex for readcount
    pthread_mutex_destroy(&(seqlock->countlock)); // Destroy the mutex for count
    pthread_mutex_destroy(&(seqlock->wrlock)); // Destroy the mutex for write lock
}
static inline void pthread_seqlock_wrlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_lock(&(seqlock->wrlock)); // Lock the write lock
    pthread_mutex_lock(&(seqlock->countlock)); // Lock the count mutex
    seqlock->count = 1; // Set the flag to indicate a writer is holding the lock
    pthread_mutex_unlock(&(seqlock->countlock)); // Unlock the count mutex
}
static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_unlock(&(seqlock->wrlock)); // Unlock the write lock
    pthread_mutex_lock(&(seqlock->countlock)); // Lock the count mutex
    seqlock->count = 0; // Set the flag to indicate no writers are holding the lock
    pthread_mutex_unlock(&(seqlock->countlock)); // Unlock the count mutex
}
static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *seqlock)
{
    while(seqlock->count == 1) {}; // Spin while a writer is holding the lock
    pthread_mutex_lock(&(seqlock->readcountlock)); // Lock the readcount mutex
    seqlock->readcount++; // Increment the number of readers holding the lock
    if (seqlock->readcount == 2) pthread_mutex_lock(&(seqlock->wrlock)); // If this is the second reader, lock the write lock
    pthread_mutex_unlock(&(seqlock->readcountlock)); // Unlock the readcount mutex
    return 1; // Return success
}
static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *seqlock)
{
    pthread_mutex_lock(&(seqlock->readcountlock)); // Lock the readcount mutex
    seqlock->readcount--; // Decrement the number of readers holding the lock
    if (seqlock->readcount == 0) pthread_mutex_unlock(&(seqlock->wrlock)); // If this is the last reader, unlock the write lock
    pthread_mutex_unlock(&(seqlock->readcountlock)); // Unlock the readcount mutex
    return 1; // Return success
}
