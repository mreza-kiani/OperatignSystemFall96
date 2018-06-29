#ifndef __NODE_H__
#define __NODE_H__

#include "global.h"

#include <semaphore.h>
#include <iostream>
#include <cstdio>
#include <pthread.h>

#define SHOULD_WAIT 0
#define CAN_EXECUTE 1
#define RESULT_READY 2

class Node
{
protected:
	sem_t* begin;
	sem_t* end;
public:
	Node(sem_t* b, sem_t* e) : begin(b), end(e) {};
	virtual void execute() = 0;
};


void* node_thread_function(void *input);

#endif