#include "output_node.h"

using namespace std;

void OutputNode::execute() {
	ofstream output;
	output.open(OUTPUT_FILE_NAME);
	output.precision(15);
	while(!end_of_input){
		result = 0;
		printf("OutputNode -> wating\n");
		for (int i = 0; i < num_of_hidden_threads; ++i){
			sem_wait(begin);
			for (int j = 0; j < num_of_hidden_threads; ++j)
				if (hidden_threads_status[j] == RESULT_READY)
				{
					printf("OutputNode -> got data %f from HiddenNode-%d\n", hidden_threads_result[j], j+1);
					result += hidden_threads_result[j] * hidden_weights[j];
					hidden_threads_status[j] = SHOULD_WAIT;
				}
			printf("OutputNode -> should wait %d times more\n", num_of_hidden_threads-i-1);
		}

		result += bios;
		output_node_result = result;
		output << result  << " vs " << data.get_function_result() << endl;
		printf("OutputNode -> has done a line calculation\n");
		sem_post(end);
	}
	output.close();
	printf("OutputNode -> data is stored in %s\n", OUTPUT_FILE_NAME);
	printf("OutputNode -> process finished\n");
}