#include "hidden_node.h"

using namespace std;

void HiddenNode::execute() {
	while(!end_of_input){
		printf("HiddenNode-%d -> waiting\n", index+1);
		// busy waiting
		while(hidden_threads_status[index] != CAN_EXECUTE);
		sem_wait(begin);

		hidden_threads_result[index] = tanh(data * wieght + bios);
		printf("HiddenNode-%d -> calculated: %f\n", index+1, hidden_threads_result[index]);

		hidden_threads_status[index] = RESULT_READY;
		sem_post(end);
	}
	printf("HiddenNode-%d -> process finished\n", index+1);
}