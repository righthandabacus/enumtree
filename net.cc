/*! @file Network data structure
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/net.cc 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#include "net.h"
#include <iostream>
#include <stdlib.h>

node::node(string* l, int i) : label(l), index(i), ncount(0)
{
	neighbour = NULL;
}

node::~node()
{
	if (label) delete label;
	free(neighbour);
}

net::net() : size(0), nodes(NULL)
{
}

net::~net()
{
	for (unsigned i=0; i<size; i++) {
		delete nodes[i];
	};
	free(nodes);
}

node* net::add(string* label)
{
	node* n=getnode(label);
	if (n==NULL) {
		n = new node(label,size);
		nodes = (node**)realloc(nodes,sizeof(node*)*(size+1));
		nodes[size++] = n;
	} else {
		delete label;
	};
	return n;
}

void node::connectTo(node* n)
{
	neighbour = (node**)realloc(neighbour, sizeof(node*)*(++ncount));
	neighbour[ncount-1] = n;
};

void net::join(node* n1, node* n2) const
{
	n1->connectTo(n2);
	n2->connectTo(n1);
}

node* net::getnode(string* s) const
{
	for (unsigned i=0; i<size; i++) {
		if (*(nodes[i]->label)==*s) {
			return nodes[i];
		};
	};
	return NULL;
};
