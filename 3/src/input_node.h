#ifndef __INPUT_NODE__
#define __INPUT_NODE__

#include "node.h"

#include <fstream>

#define INPUT_FILE_NAME "InputFile.txt"


class InputNode :public Node
{
private:
	int num_of_hidden_threads;
public:
	InputNode(sem_t* b, sem_t* e, int n) 
		:Node(b, e), num_of_hidden_threads(n) {};
	virtual void execute();	
};

#endif