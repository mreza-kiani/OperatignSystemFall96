#include "data.h"

using namespace std;

double Data::operator*(Data other) {
	return (x * other.x) + (y * other.y) + (z * other.z);
}

double Data::get_function_result() {
	return -y + sqrt(abs(y*y - 4*x*z)) / (2*x + sin(x*PI));
}
