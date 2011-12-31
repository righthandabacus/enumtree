/*! @file Implementation of a bit matrix and its factory
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/bitmatrix.h 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#ifndef __BITMATRIX_H__
#define __BITMATRIX_H__

class bitmatrix;

/*
// The factory
class matrices {
protected:
	unsigned size;
	unsigned available;
	unsigned dimension;
	bitmatrix** pool;
public:
	matrices(unsigned d);
	~matrices();
	void setdim(unsigned n);
	bitmatrix* create();
	void destroy();
	void recycle(bitmatrix* m);
};
*/

// The bit matrix
class bitmatrix {
protected:
	unsigned size;	// Size of the bit matrix
	int* vector;	// Each column
public:
	bitmatrix(unsigned s);
	bitmatrix(const bitmatrix& b);
	~bitmatrix();
	inline int& operator[](unsigned n) { return vector[n]; };
	inline const int operator[](unsigned n) const { return vector[n]; };
};

#endif
