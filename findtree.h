/*! @file Recursive procedure to enumerate valid spanning trees
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/findtree.h 21 2008-07-24T13:44:51.562294Z adrian  $
 */

#ifndef __FINDTREE_H__
#define __FINDTREE_H__

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "net.h"
#include "job.h"

struct outdata {
	std::string str;
	bool survivable;
	double depth;
	double detour;
};

bool hasloop(const bitmatrix& matrix, int node);

outdata* printtree(const net& network, const bitmatrix& matrix);

void findtree(job* j, bool atomic=true);

#endif
