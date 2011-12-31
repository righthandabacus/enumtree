/*! @file Network topology reader
 *
 *  This is a simple implementation that reads in lines of space-delimited
 *  nodes, which the first node of a line has a direct connection to all
 *  others.
 * 
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/readnet.cc 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#include <string>
#include "net.h"
#include <fstream>
//#include <iostream>

using std::ifstream;
using std::string;

/** Read in a network from an opened text file */
void readnet(net* network, ifstream& file)
{
	string line;
	while(! file.eof()) {
		getline(file,line);
		unsigned long beginpos = 0;
		unsigned long endpos;
		node* n1 = NULL;
		if (line.length()==0) continue;
		do {
			endpos = line.find_first_of(" \t",beginpos);
			if (endpos==beginpos) {
				beginpos++;
				continue;
			};
			string label = line.substr(beginpos,endpos-beginpos);
			beginpos = endpos+1;
			// First node is connected to every other nodes on the line
			node* n = network->add(new string(label));
			if (n1==NULL) {
				n1 = n;
			} else if (n != n1) {
				network->join(n1,n);	// Avoid loops
			};
		} while (endpos != string::npos);
	};
};
