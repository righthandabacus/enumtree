/*! @file Main program to find trees
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/main.cc 23 2008-07-28T16:42:21.386677Z adrian  $
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>

#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "net.h"
#include "job.h"
#include "findtree.h"
#include "bitmatrix.h"
#include "thread.h"

using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::stack;

void readnet(net* network, ifstream& file);

// Global variables
extern unsigned long treecount;
extern outdata* deepest;
extern outdata* shallow;
extern outdata* sha_sur;
extern outdata* dep_sur;
int idcount=0;				//< How many idling threads
int thcount=1;				//< Thread count
int minjob=12;				//< Min job size to regard as atomic job
char* filename=(char*)"input.txt";	//< Filename for the input data
char* outfile=NULL;			//< Filename for the input data
stack<job*> jobpool;			//< The job pool
pthread_mutex_t* jobmutex;		//< Mutex for the job pool
pthread_mutex_t* resmutex;		//< Mutex for the results
pthread_cond_t* newjob;			//< Condition: New job arrived
pthread_t* threads;			//< Array of threads
time_t begintime;			//< Time of begin
time_t endtime;				//< Time of finish

/*! print the result found so far
 */
void printresult(int signum=0)
{
	stringstream ss;
	if (signum) {
		cout << "Signal " << signum << " caught." << endl;
	};
	if (deepest)
	ss << "Deepest non-survivable tree:" << endl << deepest->str << endl << endl;
	if (shallow)
	ss << "Least depth non-survivable tree:" << endl << shallow->str << endl << endl;
	if (dep_sur)
	ss << "Deepest survivable tree:" << endl << dep_sur->str << endl << endl;
	if (sha_sur)
	ss << "Least depth survivable tree:" << endl << sha_sur->str << endl << endl;
	endtime = time(NULL);
	ss << "Totally " << treecount << " trees found." << endl;
	ss << endtime-begintime << " seconds elapsed." << endl << endl;
	if (signum) {
		cout << ss.str() << "Job pool has " << jobpool.size() << " jobs pending." << endl;
	} else if (outfile) {
		ofstream file(outfile);
		file << ss.str();
		file.close();
	} else {
		cout << ss.str();
	};
};

/*! the main program
 */
int main(int argc, char** argv)
{
	// Process command line options
	int opt;
	while (-1 != (opt=getopt(argc, argv, "t:i:m:o:"))) {
		switch(opt) {
			case 't':
				thcount = strtol(optarg,NULL,0);
				if (thcount<=0) thcount=1;
				break;
			case 'i':
				filename = optarg;
				break;
			case 'm':
				minjob = strtol(optarg,NULL,0);
				if (minjob <= 0) minjob = 0;
				break;
			case 'o':
				outfile = optarg;
				break;
		};
	};
	argc -= optind;
	argv += optind;

	// Prepare variables
	net* network = new net();	//< Network topology
	string rootlbl;			//< Label of the root node
	ifstream file(filename);	//< Topology data file

	// Prepare threads
	threads = new pthread_t[thcount];
	jobmutex = new pthread_mutex_t;
	pthread_mutex_init(jobmutex,NULL);
	resmutex = new pthread_mutex_t;
	pthread_mutex_init(resmutex,NULL);
	newjob = new pthread_cond_t;
	pthread_cond_init(newjob,NULL);
	for (int i=0; i<thcount; i++) {
		if (pthread_create(&threads[i], NULL, thmain, NULL)) {
			cerr << "Failed creating thread #" << i<< endl;
			return 1;
		};
	};
	
	// Initialize the topology
	if (!file.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return 1;
	};
	getline(file,rootlbl);		// First line: The root node
	readnet(network,file);		// Read the rest: The whole topology
	node* root = network->getnode(&rootlbl);	// root node object
	bitmatrix* matrix = new bitmatrix(network->getsize());// initial incident matrix
	for (int i=0; i<network->getsize(); i++) {
		(*matrix)[i] = -1;
	};
	if (minjob==0) {
		minjob = network->getsize() - 4;
	};

	// Set-up the signal handlers, run algorithm, and reset signal
	signal(SIGINT, printresult);
	job* j = new job(network, root, matrix, 0);
	pthread_mutex_lock(jobmutex);
	begintime = time(NULL);
	jobpool.push(j);
	idcount=thcount;
	pthread_mutex_unlock(jobmutex);
	pthread_cond_signal(newjob);
	for (int i=0; i<thcount; i++) {
		pthread_join(threads[i],NULL);
	};
	pthread_mutex_destroy(resmutex);
	pthread_mutex_destroy(jobmutex);
	pthread_cond_destroy(newjob);
	signal(SIGINT, SIG_DFL);

	// Print result after finish
	printresult();

	// Clean up
	delete network;
	if (deepest != shallow) delete deepest;
	delete shallow;
       	if (dep_sur != sha_sur) delete dep_sur;
	delete sha_sur;
	delete[] threads;
	delete jobmutex;
	delete resmutex;
	delete newjob;
	return 0;
};
