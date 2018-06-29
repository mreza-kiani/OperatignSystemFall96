#include "node.h"

using namespace std;

void* node_thread_function(void *thread_arg) {
	Node* node = (Node*)thread_arg;
	node->execute();
}