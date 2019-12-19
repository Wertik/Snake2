#include "Point.hpp"

Point Point::operator+(Point arg) {
	return Point(x + arg.x, y + arg.y);
}

Point Point::operator-(Point arg) {
	return Point(x - arg.x, y - arg.y);
}

Point Point::operator*(Point arg) {
	return Point(arg.x * x, arg.y * y);
}

Point Point::operator+=(Point arg) {
	return Point(x + arg.x, y + arg.y);
}

Point Point::operator=(const Point& arg) {
	return Point(arg.x, arg.y);
}

bool Point::operator==(Point arg) {
	return (x == arg.x && y == arg.y);
}

Point::Point() {
	x = 0;
	y = 0;
}

Point::Point(int _x, int _y) {
	x = _x;
	y = _y;
}