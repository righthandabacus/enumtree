/*! @file Class header for an atomic job in the tree enumeration algorithm
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/job.h 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#ifndef __JOB_H__
#define __JOB_H__

#include "net.h"
#include "bitmatrix.h"

/*! A job object for the tree enumeration
 */
class job {
public:
	net* network;	//< The network (topology)
	node* root;	//< The root node
	bitmatrix* A;	//< The adjacency matrix of the tree
	int index;	//< Index to the current position
public:
	job(net* n, node* r, bitmatrix* A, int i);
	~job();
};

#endif
