#ifndef __DATA_H__
#define __DATA_H__ 

#include <cmath>

#define PI 3.141592653589793

class Data
{
private:
	double x;
	double y;
	double z;
public:
	Data(){}
	Data(double _x, double _y, double _z)
		: x(_x), y(_y), z(_z) {}
	double operator*(Data other);
	double get_function_result();
	
	void set_x(double _x) {x = _x;}
	void set_y(double _y) {y = _y;}
	void set_z(double _z) {z = _z;}
};

#endif