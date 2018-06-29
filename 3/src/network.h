#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "global.h"
#include "node.h"
#include "data.h"
#include "input_node.h"
#include "hidden_node.h"
#include "output_node.h"
#include "stats_node.h"

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>
#include <semaphore.h>

class Network
{
private:
	Node* input_node;
	std::vector<Node*> hidden_nodes;
	Node* output_node;
	Node* stats_node; // emtiazi

	sem_t input, hidden, output, stats;

	pthread_t input_thread;
	std::vector<pthread_t> hidden_threads;
	pthread_t output_thread;
	pthread_t stats_thread; // emtiazi

	void init_input_node();
	void init_hidden_nodes();
	void init_output_node();
	void init_stats_node();
	void init_semaphores();

	void run_input_node();
	void run_hidden_nodes();
	void run_output_node();
	void run_stats_node();
public:
	Network() {};
	void init();
	void run();
};

#endif