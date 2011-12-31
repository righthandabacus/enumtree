/*! @file Class header for the network data structure
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/net.h 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#ifndef __NET_H__
#define __NET_H__

#include <string>

using std::string;

class node {
public:
	node(string* l=NULL,int i=0);
	~node();
	void connectTo(node* n);
public:
	string* label;		//< Label of this node
	unsigned index;		//< Index of this node
	unsigned ncount;	//< Neighbour count
	node** neighbour;	//< Its neighbourhood
};

class net {
public:
	net();
	~net();
	node* add(string*);
	void join(node*,node*) const;
	inline node* getnode(int i) const { return nodes[i]; };
	node* getnode(string*) const;
	inline int getsize() const { return size; };
protected:
	unsigned size;		//< Holds the total number of nodes
	node** nodes;		//< The array of nodes
};

#endif
