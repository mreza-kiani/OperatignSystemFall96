#ifndef __STATS_NODE__
#define __STATS_NODE__

#include "global.h"
#include "node.h"
#include "data.h"

#include <cmath>

class StatsNode :public Node
{
private:
	double result;
	int num_of_outputs;
public:
	StatsNode(sem_t* b, sem_t* e);
	virtual void execute();
};

#endif