/*! @file Main program for threads
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/thread.cc 20 2008-07-24T12:43:57.847093Z adrian  $
 */

#include "thread.h"
#include "findtree.h"
#include "job.h"

#include <iostream>
#include <pthread.h>
#include <cmath>

using std::cout;
using std::endl;
using std::pow;

/*! Thread controller
 */
void* thmain(void* arg)
{
	for(;;) {
		pthread_mutex_lock(jobmutex);
		// Wait for a new job or a termination signal
		while (jobpool.empty() && idcount!=thcount) {
			pthread_cond_wait(newjob,jobmutex);
		};
		// If there is a new job, handle it
		if (!jobpool.empty()) {
			job* j = jobpool.top();
			jobpool.pop();
			idcount--;
			bool atomic = ((j->network->getsize())-(j->index) <= minjob);
			pthread_mutex_unlock(jobmutex);
			findtree(j, atomic);
			pthread_mutex_lock(jobmutex);
			idcount++;
			pthread_mutex_unlock(jobmutex);
			delete j;
		} else {
			// All job finished, tell all other threads and terminate
			pthread_mutex_unlock(jobmutex);
			pthread_cond_signal(newjob);
			break;
		};
	};
	return NULL;	// equiv: pthread_exit(NULL);
};
