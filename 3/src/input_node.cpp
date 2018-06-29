#include "input_node.h"

using namespace std;

void InputNode::execute() {
	ifstream in;
	in.open(INPUT_FILE_NAME);
	int line = 1;

	while(in.good()) {
		printf("InputNode -> wating\n");
		sem_wait(begin);
		printf("InputNode -> executing for input line %d\n", line);

		double x_weight, y_weight, z_weight;
		in >> x_weight >> y_weight >> z_weight;
		printf("InputNode -> got: %f %f %f\n", x_weight, y_weight, z_weight);

		data.set_x(x_weight);
		data.set_y(y_weight);
		data.set_z(z_weight);

		
		for (int i = 0; i < num_of_hidden_threads; ++i)
			hidden_threads_status[i] = CAN_EXECUTE;

		for (int i = 0; i < num_of_hidden_threads; ++i)
			sem_post(end);
		line++;
	}
	end_of_input = true;
	printf("InputNode -> process finished\n");
	in.close();
}
