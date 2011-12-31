/*! @file Main program for threads
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/thread.h 20 2008-07-24T12:43:57.847093Z adrian  $
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <stack>
#include <pthread.h>
#include "job.h"

using std::stack;

extern int idcount;			//< Count of idling threads
extern int thcount;			//< Total number of threads
extern stack<job*> jobpool;		//< Job pool
extern pthread_mutex_t* jobmutex;	//< Mutex for the job pool
extern pthread_cond_t* newjob;		//< Condition: New job arrived
extern int minjob;			//< Minimum job size to regard as an atomic job

// Main function for a thread
void* thmain(void* arg);

#endif
