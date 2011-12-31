/*! @file Recursive procedure to enumerate valid spanning trees
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/findtree.cc 23 2008-07-28T16:42:21.386677Z adrian  $
 */

#include "findtree.h"
#include "cirq.h"
#include "bitmatrix.h"
#include <stack>

using std::cout;
using std::endl;
using std::stack;
using std::stringstream;
using std::string;

unsigned long treecount=0;
outdata* deepest=NULL;
outdata* shallow=NULL;
outdata* sha_sur=NULL;
outdata* dep_sur=NULL;

extern stack<job*> jobpool;
extern pthread_mutex_t* jobmutex;
extern pthread_mutex_t* resmutex;
extern pthread_cond_t* newjob;

/** Loop verification algorithm: Based on a said node, see if we have a loop
 *  about it. The search is from this node onward to trace back to its
 *  ancestors. A loop is found if we reached that node again
 */
bool hasloop(const bitmatrix& matrix, int node)
{
	int curnode = node;
	int prev;
	for (;;) {
		// Search for the parent of curnode
		prev = matrix[curnode];
		if (prev == -1) return false;		// No more parent
		if (prev == curnode) return false;	// Only root, no loop
		if (prev == node) return true;		// A loop is found
		//if (prev > node) return true;		// No more parent
		curnode = prev;
	};
};

/*! Procedure to print the tree, calculate the average depth and
 *  also the detours. The detours are found using the Dijkstra's
 *  algorithm on each node's parent.
 */
outdata* printtree(const net& network, const bitmatrix& matrix)
{
	stringstream ss;

	// Calculate the average tree depth
	int depth[network.getsize()];			//< Depth of a node from root
	cirq<int> nodeq(network.getsize());		//< Queue of nodes to examine
	int curnode;					//< The current node

	for (int i=0; i<network.getsize(); i++) {	// Find the root node
		if (matrix[i]==i) {
			depth[i] = 1;
			nodeq.push(i); // enqueue root node if found
		} else {
			depth[i] = 0;
		};
	};
	while (! nodeq.empty()) {			// Calculate depth until queue exhausted
		curnode = nodeq.front();		// For every node reachable so far
		nodeq.pop();
		bool headprint=false;			//< for printing use
		for (int i=0; i<network.getsize(); i++) {
			// Find the children of this node
			if (matrix[i]!=curnode || curnode==i) {	// Find the children
				continue;
			};
			// print the children if there are any
			if (!headprint) {
				ss << *(network.getnode(curnode)->label) << " ->";
				headprint=true;
			};
			ss << " " << *(network.getnode(i)->label);
			// and calculate the individual depth
			depth[i] = depth[curnode]+1;	// mark their depth
			nodeq.push(i);			// and put to the queue
		};
		if (headprint) ss << endl;
	};
	long double avgdepth = 0.0L;			// Find average depth
	for (int i=0; i<network.getsize(); i++) {
		avgdepth += depth[i]-1.0L;
	};
	avgdepth /= network.getsize()-1.0L;
	ss << "Average hop count from root = " << avgdepth << "\n";

	// Check if it is worthwhile to check its survivability
	if (deepest && (deepest->depth > avgdepth) &&
	    shallow && (shallow->depth < avgdepth) &&
	    dep_sur && (dep_sur->depth > avgdepth) &&
	    sha_sur && (sha_sur->depth < avgdepth)) {
		return NULL;
	};

	// Verify the network is survivable
	bool ischild[network.getsize()];
	int netsize = network.getsize();
	unsigned long detourlen = 0;
	unsigned detourcount = 0;
	bool survivable = true;
	for (int i=0; i<network.getsize(); i++) {	// For every node i
		if (matrix[i]==i) continue;		// that is not the root node
		int parent = matrix[i];			// Find its parent
		int childcount = 0;			// and children
		int reachcount = 0;
		for (int j=network.getsize()-1; j>=0; j--) {
			if (matrix[j]==i) {
				ischild[j] = true;
				childcount++;
			} else {
				ischild[j] = false;
			};
		};
		if (childcount==0) continue;		// No detour if no children
		// Find routes from parent to children, using Dijkstra's algorithm
		int routes[netsize][netsize+1];		// Routes to each node
		nodeq.push(parent);			// Enqueue parent node
		for (int j=0; j<netsize; j++) {
			routes[j][0]=-1;
		};
		routes[parent][0] = routes[i][0] = i;
		routes[parent][1] = parent;			// Set the route to parent node
		routes[parent][2] = routes[i][1] = -1;
		while(! nodeq.empty()) {			// Find route until queue exhausted
			int curnode = nodeq.front();
			nodeq.pop();
			node* n = network.getnode(curnode);
			for (unsigned k=0; k<(n->ncount); k++) {
				int neighnode = n->neighbour[k]->index;		// For every neighbour node
				if (routes[neighnode][0]!=-1) continue;		// that has not yet seen and
				if (matrix[neighnode]==curnode) continue;	// has no conflict with the tree,
				int j=0;					// Copy the route to this neighbour
				do {
					routes[neighnode][j] = routes[curnode][j];
				} while (routes[curnode][++j]!=-1);
				routes[neighnode][j] = neighnode;		// Update the route to this node
				routes[neighnode][j+1] = -1;
				if (ischild[neighnode]) {
					if (++reachcount == childcount) {	// Keep track on the num of reached nodes
						nodeq.clear();			// and make early termination if appropriate
						break;
					};
				};
				nodeq.push(neighnode);				// Check this neighbour as well
			};
		};
		for (int j=0; j<network.getsize(); j++) {	// Print the detours
			if (! ischild[j]) continue;
			ss << "Detour from " << *(network.getnode(i)->label) << " to " << *(network.getnode(j)->label) << " :";
			if (routes[j][0]!=-1) {
				int k=0;
				do {
					ss << ' ' << *(network.getnode(routes[j][k])->label);
				} while (routes[j][++k]!=-1);
				ss << endl;
				detourcount++;
				detourlen += k;
			} else {
				ss << " Does not exist." << endl;
				survivable = false;
			};
		};
	};
	// Print the summary on survivability
	outdata* D = new outdata;
	D->depth = avgdepth;
	if ( (D->survivable = (survivable && detourcount)) ) {
		D->detour = (long double)(detourlen)/(long double)(detourcount);
		ss << "This is a survivable network with average detour length of " << D->detour << endl;
	} else {
		ss << "This is not a survivable network." << endl;
	};
	D->str = ss.str();
	return D;
};

/*! Tree finding algorithm. Kruskal's algorithm is used to build the spanning
 *  tree.
 *    j: The job specification
 *    atomic: Whether the job is atomic, or it should create sub-jobs, default true
 */
void findtree(job* j, bool atomic)
{
	// Localise variable for clearner code
	int nodeidx = j->index;
	net& network = *(j->network);
	bitmatrix& A = *(j->A);

	// Tell the estimate size, only on first run
	static int rootidx;
	if (nodeidx==0) {
		rootidx = j->root->index;
		long double estimate=1.00;
		for (int i=0; i<network.getsize(); i++) {
			if (i==rootidx) continue;
			estimate *= network.getnode(i)->ncount;
		};
		cout << "Size of solution space: " << estimate << endl;;
	};
	// Case of root node
	if (nodeidx==rootidx) {
		// Root node shouldn't have an incident edge, move on
		nodeidx++;
	};
	// Terminating case: we found a good spanning tree
	if (nodeidx==network.getsize()) {
		// On completing the last column, we formed a tree
		A[rootidx]=rootidx;
		outdata* d = printtree(network,A);
		pthread_mutex_lock(resmutex);
		treecount++;
		if (d) {
			if ((deepest==NULL || d->depth > deepest->depth) && !d->survivable) {
				if (deepest && shallow!=deepest) { delete deepest; };
				deepest = d;
			};
			if ((shallow==NULL || d->depth < shallow->depth) && !d->survivable) {
				if (shallow) { delete shallow; };
				shallow = d;
			};
			if ((dep_sur==NULL || d->depth > dep_sur->depth) && d->survivable) {
				if (dep_sur && sha_sur !=dep_sur) { delete dep_sur; };
				dep_sur = d;
			};
			if ((sha_sur==NULL || d->depth < sha_sur->depth) && d->survivable) {
				if (sha_sur) { delete sha_sur; };
				sha_sur = d;
			};
			if (sha_sur != d && dep_sur != d && shallow != d && deepest != d) {
				delete d;
			};
		};
		pthread_mutex_unlock(resmutex);
		return;
	};
	// Loop for setting an incident edge to this node (Kruskal's algorithm)
	node& thisnode = *(network.getnode(nodeidx));
	for (unsigned k=0; k<(thisnode.ncount); k++) {
		int i = thisnode.neighbour[k]->index;
		A[nodeidx] = i;				// try edges from each neighbour nodes
		if (hasloop(A,nodeidx)) continue;	// then check to see if there is any loop
		if (atomic) {				// If no loop and it is an atomic job,
			j->index = nodeidx+1;
			findtree(j,true);		// move on to next column and recur
		} else {				// If no loop but it is not atomic
			job* subjob = new job(j->network, j->root, new bitmatrix(A), nodeidx+1);
			pthread_mutex_lock(jobmutex);
			jobpool.push(subjob);		// put a new job into the job pool
			pthread_mutex_unlock(jobmutex);
			pthread_cond_signal(newjob);
		};
	};
	A[nodeidx] = -1;
};
