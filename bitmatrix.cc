/*! @file Implementation of a bit matrix and its factory
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/bitmatrix.cc 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#include <stdlib.h>
#include "bitmatrix.h"

bitmatrix::bitmatrix(unsigned s) : size(s)
{
	vector = new int[s];
	for (unsigned i=0; i<s; i++) {
		vector[i]=-1;
	};
};

bitmatrix::bitmatrix(const bitmatrix& b)
{
	size = b.size;
	vector = new int[size];
	for (unsigned i=0; i<size; i++) {
		vector[i] = b.vector[i];
	};
};

bitmatrix::~bitmatrix()
{
	delete[] vector;
};
