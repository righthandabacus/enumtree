/*! @file A job in the tree enumeration algorithm
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/job.cc 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#include "job.h"

job::job(net* n, node* r, bitmatrix* b, int i)
	: network(n), root(r), A(b), index(i)
{
};

job::~job()
{
	delete A;
};
