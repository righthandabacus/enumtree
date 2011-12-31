/*! @file Template of a circular queue (aka circular buffer)
 *
 *  vim:set ts=8 sw=8 filetype=cpp noai nosi cin nu noet nolbr sm sbr=">" ff=unix syntax=cpp noic nowrap:
 *  $Id: /enumtree/cirq.h 18 2008-07-23T18:57:51.788775Z adrian  $
 */

#ifndef __CIRQ_H__
#define __CIRQ_H__

// The circular queue class
template <class _T>
class cirq {
public:
	// Constructor: initialize an array of _T
	cirq(unsigned s) : qsize(s), head(0), tail(0)
	{	items = new _T[qsize]; };

	// Destructor
	~cirq()
	{	delete[] items; };

	// Insert an object to the queue
	inline void push(_T item)
	{
		items[tail++] = item;
		if (tail == qsize) {
			tail = 0;
		};
	};

	// Remove an object from queue
	inline void pop()
	{
		head = head++;
		if (head == qsize) {
			head = 0;
		};
	};

	// Return the object at the head of queue
	inline _T front() const
	{	return items[head]; };

	// Tell whether this queue is empty. This assumes the queue never overflew.
	inline bool empty() const
	{	return (head==tail); };

	// Tell how many objects are there in this queue
	inline unsigned size() const
	{	return (head<=tail)?(tail-head):(qsize+tail-head); };

	// Empty the queue
	inline void clear()
	{	head = tail = 0; };
protected:
	unsigned qsize;		//< Size of the circular queue
	unsigned head;		//< Position of the head
	unsigned tail;		//< Position of the tail
	_T* items;		//< Array of items
};

#endif
