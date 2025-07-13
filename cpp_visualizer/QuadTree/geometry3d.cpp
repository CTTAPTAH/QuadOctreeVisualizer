#include "geometry3d.h"

// Точка
Point3::Point3(double x_, double y_, double z_) {
	x = x_;
	y = y_;
	z = z_;
}
bool Point3::operator==(const Point3& p) const {
	return x == p.x and y == p.y and z == p.z;
}

// Бокс
Box3::Box3(double x_, double y_, double z_, double w_, double h_, double d_) {
	x = x_; y = y_; z = z_;
	w = w_; h = h_; d = d_;
}

float Box3::getCenterX() const { return x + w / 2.0f; }
float Box3::getCenterY() const { return y + h / 2.0f; }
float Box3::getCenterZ() const { return z + d / 2.0f; }
float Box3::getHalfW() const { return w / 2.0f; }
float Box3::getHalfH() const { return h / 2.0f; }
float Box3::getHalfD() const { return d / 2.0f; }

bool Box3::contains(const Point3& p) const {
	return (p.x >= x) and (p.x < x + w) and
		(p.y >= y) and (p.y < y + h) and
		(p.z >= z) and (p.z < z + d);
}
bool Box3::intersect(const Box3& b) const {
	return !(x + w < b.x or b.x + b.w < x or
		y + h < b.y or b.y + b.h < y or
		z + d < b.z or b.z + b.d < z);
}