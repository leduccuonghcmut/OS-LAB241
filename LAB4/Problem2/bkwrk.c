#include "bktpool.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/syscall.h>


#define INFO
#define WORK_THREAD

static unsigned int next_worker = 0;

void *bkwrk_worker(void *arg) {
    sigset_t set;
    int sig;
    int s;
    int i = *((int *)arg);
    struct bkworker_t *wrk = &worker[i];

    /* Taking the mask for waking up */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGQUIT);

#ifdef DEBUG
    fprintf(stderr, "worker %i start living tid %d \n", i, getpid());
    fflush(stderr);
#endif

    while (1) {
        /* wait for signal */
        s = sigwait(&set, &sig);
        if (s != 0)
            continue;

#ifdef INFO
        fprintf(stderr, "worker wake %d up\n", i);
#endif

        /* Busy running */
        if (wrk->func != NULL)
            wrk->func(wrk->arg);

        /* Advertise I DONE WORKING */
        wrkid_busy[i] = 0;
        worker[i].func = NULL;
        worker[i].arg = NULL;
        worker[i].bktaskid = -1;
    }
}

int bktask_assign_worker(unsigned int bktaskid, unsigned int wrkid) {
    if (wrkid >= MAX_WORKER)
        return -1;

    struct bktask_t *tsk = bktask_get_byid(bktaskid);

    if (tsk == NULL)
        return -1;

    /* Advertise I AM WORKING */
    wrkid_busy[wrkid] = 1;

    worker[wrkid].func = tsk->func;
    worker[wrkid].arg = tsk->arg;
    worker[wrkid].bktaskid = bktaskid;

    printf("Assign tsk %d wrk %d \n", tsk->bktaskid, wrkid);
    return 0;
}

int bkwrk_create_worker() {
    unsigned int i;

    for (i = 0; i < MAX_WORKER; i++) {

        pid_t pid = fork();
        if (pid == 0) {
            /* Child process - worker */
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigaddset(&set, SIGQUIT);
            sigprocmask(SIG_BLOCK, &set, NULL);

            int wrkid = i;
            bkwrk_worker(&wrkid);
            exit(0);
        } else if (pid > 0) {
            /* Parent process */
            wrkid_tid[i] = pid;
#ifdef INFO
            fprintf(stderr, "bkwrk_create_worker got worker %u\n", pid);
#endif
        } else {
            /* Fork thất bại */
            perror("fork");
            return -1;
        }
    }

    return 0;
}

int bkwrk_get_worker() {
    unsigned int start = next_worker;

    /* Loop through workers in a circular FIFO manner */
    while (1) {
        if (!wrkid_busy[next_worker]) {
            unsigned int selected_worker = next_worker;
            next_worker = (next_worker + 1) % MAX_WORKER;
            return selected_worker;
        }
        next_worker = (next_worker + 1) % MAX_WORKER;

        /* If we have scanned all workers, return an error */
        if (next_worker == start)
            return -1;
    }
}

int bkwrk_dispatch_worker(unsigned int wrkid) {
#ifdef WORK_THREAD
    unsigned int tid = wrkid_tid[wrkid];

    /* Invalid task */
    if (worker[wrkid].func == NULL)
        return -1;

#ifdef DEBUG
    fprintf(stderr, "brkwrk dispatch wrkid %d - send signal %u \n", wrkid, tid);
#endif

    syscall(SYS_tkill, tid, SIGUSR1);
#else
    pid_t pid = wrkid_tid[wrkid];

    /* Invalid task */
    if (worker[wrkid].func == NULL)
        return -1;

#ifdef DEBUG
    fprintf(stderr, "bkwrk dispatch wrkid %d - send signal %d \n", wrkid, pid);
#endif

    kill(pid, SIGUSR1);
#endif
}

