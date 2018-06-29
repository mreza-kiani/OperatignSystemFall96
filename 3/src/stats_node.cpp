#include "stats_node.h"

using namespace std;

StatsNode::StatsNode(sem_t* b, sem_t* e) : Node(b, e) {
	result = 0;
	num_of_outputs = 0;
}

void StatsNode::execute() {
	while(!end_of_input){
		printf("StatsNode -> wating\n");
		sem_wait(begin);
		num_of_outputs ++;
		printf("StatsNode -> data %d recieved\n", num_of_outputs);
		double x = output_node_result - data.get_function_result();
		result += x * x;

		sem_post(end);
	}
	printf("StatsNode -> standard deviation: %f\n", sqrt(result/num_of_outputs));
	printf("StatsNode -> process finished\n");
}