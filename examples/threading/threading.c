#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    ThreadData* thread_func_args = (ThreadData*) thread_param;
    
    //Wait to obtain the lock
    usleep(thread_func_args->wait_to_obtain_ms*1000);
    int rc = pthread_mutex_lock(thread_func_args->lock);
    
    //if error, return the data for joiner to free memory
    if(rc != 0)
    {
    	ERROR_LOG("Failed to get lock, error=%d", rc);
    	return thread_param;
    }
    
    //Got the lock, wait til release
    usleep(thread_func_args->wait_to_release_ms*1000);
    rc = pthread_mutex_unlock(thread_func_args->lock);
    
    //if error, return the data for joiner to free memory
    if(rc != 0)
    {
    	ERROR_LOG("Failed to unlock, error=%d", rc);
    	return thread_param;
    }
    
    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    
    ThreadData* threadData = malloc(sizeof(ThreadData));
    threadData->lock = mutex;
    threadData->wait_to_obtain_ms = wait_to_obtain_ms;
    threadData->wait_to_release_ms = wait_to_release_ms;
    threadData->thread_complete_success = false;
     
    int rc = pthread_create(thread, NULL, threadfunc, (void*) threadData);
    
    //If couldn't make thread, print error
    if(rc != 0)
    {
    	ERROR_LOG("Failed to create thread error=%d", rc);
    	return false;
    }
    return true;
}

