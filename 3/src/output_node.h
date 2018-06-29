#ifndef __OUTPUT_NODE__
#define __OUTPUT_NODE__

#include "node.h"

#include <vector>
#include <fstream>

#define OUTPUT_FILE_NAME "OutputFile.txt"

class OutputNode :public Node
{
private:
	double bios;
	std::vector<double> hidden_weights;
	int num_of_hidden_threads;
	double result;
public:
	OutputNode(sem_t* b, sem_t* e, double bi, std::vector<double> hw, int n) 
		:Node(b, e), bios(bi), hidden_weights(hw), num_of_hidden_threads(n) {};
	virtual void execute();
};

#endif