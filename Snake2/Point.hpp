#pragma once

class Point {
public:
	int x;
	int y;

	Point();
	Point(int x, int y);

	Point operator+(Point arg);
	Point operator-(Point arg);
	Point operator*(Point arg);
	Point operator+=(Point arg);
	Point operator=(const Point& arg);
	bool operator==(Point arg);
};