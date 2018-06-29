#include "network.h"

using namespace std;

void Network::init() {
	init_hidden_nodes();
	init_output_node();
	init_input_node();
	init_stats_node(); // emtiazi
	init_semaphores();
}

void Network::run() {
	run_input_node();
	run_hidden_nodes();
	run_output_node();
	run_stats_node();
}

void Network::init_input_node() {
	input_node = new InputNode(&input, &hidden, hidden_nodes.size());
}

void Network::init_hidden_nodes() {
	int num_of_hidden_threads;
	cout << "Enter num of hidden nodes: ";
	cin >> num_of_hidden_threads;

	for (int i = 0; i < num_of_hidden_threads; ++i){
		hidden_threads_status.push_back(0);
		hidden_threads_result.push_back(0);
		hidden_threads.push_back(pthread_t());
	}

	vector<double> hidden_nodes_bios(num_of_hidden_threads);
	for (int i = 0; i < num_of_hidden_threads; ++i){
		cout << "Enter hidden node " << i+1 << " bios: ";
		cin >> hidden_nodes_bios[i];
	}

	double x_weight, y_weight, z_weight;
	for (int i = 0; i < num_of_hidden_threads; ++i){
		cout << "Enter hidden node " << i+1 << " x, y, z weight: ";
		cin >> x_weight >> y_weight >> z_weight;
		hidden_nodes.push_back(new HiddenNode(&hidden, &output, hidden_nodes_bios[i],
			Data(x_weight, y_weight, z_weight), i));
	}
}

void Network::init_output_node() {
	int num_of_hidden_threads = hidden_nodes.size();

	double output_bios;
	cout << "Enter output node bios: ";
	cin >> output_bios;

	vector<double> output_weights(num_of_hidden_threads);
	for (int i = 0; i < num_of_hidden_threads; ++i){
		cout << "Enter output weight for hidden node " << i+1 << ": ";
		cin >> output_weights[i];
	}
	output_node = new OutputNode(&output, &stats, output_bios, output_weights, num_of_hidden_threads);

}

void Network::init_stats_node() {
	stats_node = new StatsNode(&stats, &input);
}

void Network::init_semaphores() {
	sem_init(&input, 0, 1);
	sem_init(&hidden, 0, 0);
	sem_init(&output, 0, 0);
	sem_init(&stats, 0, 0);
}


void Network::run_input_node() {
 	printf("creating InputThread\n");
 	pthread_create(&input_thread, NULL, node_thread_function, input_node);
}

void Network::run_hidden_nodes() {
	for (int i = 0; i < hidden_nodes.size(); ++i)
	{
	 	printf("creating HiddenThread-%d\n", i+1);
	 	pthread_create(&hidden_threads[i], NULL, node_thread_function, hidden_nodes[i]);
	}
}

void Network::run_output_node() {
 	printf("creating OutputThread\n");
 	pthread_create(&output_thread, NULL, node_thread_function, output_node);
}

void Network::run_stats_node() {
 	printf("creating StatsThread\n");
 	pthread_create(&stats_thread, NULL, node_thread_function, stats_node);
}