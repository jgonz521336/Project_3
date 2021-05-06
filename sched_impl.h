#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__

#include "list.h"
#include "scheduler.h"
#include <pthread.h>
#include <semaphore.h>

struct thread_info {
	/*...Fill this in...*/
	list_t* queue;
        list_elem_t* queueData;
        sem_t runWorker;
};

struct sched_queue {
	/*...Fill this in...*/
	list_elem_t* currentWorker;
        list_elem_t* nextWorker;
	list_t* list;
};

#endif /* __SCHED_IMPL__H__ */
