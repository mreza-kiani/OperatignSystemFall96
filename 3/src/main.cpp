#include "network.h"
#include <pthread.h>
#include <vector>
#include <iostream>

using namespace std;

// declaring global varriables
Data data;
vector<double> hidden_threads_result;
vector<int> hidden_threads_status;
bool end_of_input = false;
double output_node_result;

int main() {
	cout.precision(15);
	
	Network neural_network;
	neural_network.init();
	neural_network.run();

   	pthread_exit(NULL);
	return 0;
}
