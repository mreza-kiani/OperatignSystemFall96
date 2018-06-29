#ifndef __HIDDEN_NODE__
#define __HIDDEN_NODE__

#include "node.h"
#include "data.h"
#include <cmath>

class HiddenNode :public Node
{
private:
	Data wieght;
	double bios;
	int index;
public:
	HiddenNode(sem_t* b, sem_t* e, double bi, Data w, int i) 
		:Node(b, e), bios(bi), wieght(w), index(i) {}
	virtual void execute();
};

#endif