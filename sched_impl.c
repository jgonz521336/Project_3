#include "scheduler.h"
#include "sched_impl.h"

/* Fill in your scheduler implementation code below: */
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <assert.h>
sem_t controlSemaphore;
sem_t cpuSemaphore;
sem_t emptySemaphore;


static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	/*...Code goes here...*/
	info->queue = queue->list;
        info->queueData = NULL;
}

static void destroy_thread_info(thread_info_t *info)
{
	/*...Code goes here...*/
	 free(info->queueData);
}

/*...More functions go here...*/

static void enter_sched_queue(thread_info_t *info)
{
        sem_wait(&controlSemaphore);
        info->queueData = (list_elem_t*)malloc(sizeof(list_elem_t));
        list_elem_init(info->queueData, (void*)info);
        list_insert_tail(info->queue, info->queueData);
        if(list_size(info->queue) == 1)
                sem_post(&emptySemaphore);
        sem_init(&info->runWorker,0,0);
}

static void leave_sched_queue(thread_info_t *info)
{
        list_remove_elem(info->queue, info->queueData);
        sem_post(&controlSemaphore);
}

static void wait_for_cpu(thread_info_t *info)
{
        sem_wait(&info->runWorker);
}

static void release_cpu(thread_info_t *info)
{
        sem_post(&cpuSemaphore);
        sched_yield();
}


static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	/*...Code goes here...*/
	      if (queue_size <= 0) {
                exit(-1); 
        }
        queue->currentWorker = NULL;
        queue->nextWorker = NULL;
        queue->list = (list_t*) malloc(sizeof(list_t));
        list_init(queue->list);
        sem_init(&controlSemaphore, 0, queue_size);
        sem_init(&cpuSemaphore,0,0);
        sem_init(&emptySemaphore,0,0);
}

static void destroy_sched_queue(sched_queue_t *queue)
{
	/*...Code goes here...*/
	    list_elem_t * temp;
        while ((temp = list_get_head(queue->list)) != NULL) {
                list_remove_elem(queue->list, temp);
                free(temp);
        }
        free(queue->list);
}

/*...More functions go here...*/

static void wake_up_worker(thread_info_t *info)
{
        sem_post(&info->runWorker);
}

static void wait_for_worker(sched_queue_t *queue)
{
        sem_wait(&cpuSemaphore);
}

static thread_info_t * next_worker_rr(sched_queue_t *queue)
{
        if(list_size(queue->list) == 0) {
                return NULL;
        }

        if(queue->currentWorker == NULL) {
                queue->currentWorker = list_get_head(queue->list);
        } else if (queue->nextWorker == NULL) {
                if (queue->currentWorker == list_get_tail(queue->list)) {
                        queue->currentWorker = list_get_head(queue->list);
                } else {
                        queue->currentWorker = list_get_tail(queue->list); 
                }
        } else {
                queue->currentWorker = queue->nextWorker;
        }

        queue->nextWorker = queue->currentWorker->next;
        return (thread_info_t*) queue->currentWorker->datum;
}

static thread_info_t * next_worker_fifo(sched_queue_t *queue) {
        if(list_size(queue->list) == 0) {
                return NULL;
        }
        else {
                return (thread_info_t*) (list_get_head(queue->list))->datum;
        }
}

static void wait_for_queue(sched_queue_t *queue)
{
        sem_wait(&emptySemaphore);
}


/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info , enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu}, 
	{ init_sched_queue, destroy_sched_queue , wake_up_worker, wait_for_worker, next_worker_fifo, wait_for_queue} },
sched_rr = {
	{ init_thread_info, destroy_thread_info , enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu }, 
	{ init_sched_queue, destroy_sched_queue , wake_up_worker, wait_for_worker, next_worker_rr, wait_for_queue } };
