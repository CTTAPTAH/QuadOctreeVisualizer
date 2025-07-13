#pragma once
struct Point3 {
	double x = 0, y = 0, z = 0;
	double vx = 100, vy = 100, vz = 100;

	Point3() = default;
	Point3(double x_, double y_, double z_);
	bool operator==(const Point3& p) const;
};
struct Box3 {
	double x = 0, y = 0, z = 0;
	double w = 0, h = 0, d = 0;

	Box3(double x_, double y_, double z_, double w_, double h_, double d_);

	float getCenterX() const;
	float getCenterY() const;
	float getCenterZ() const;

	float getHalfW() const;
	float getHalfH() const;
	float getHalfD() const;

	bool contains(const Point3& p) const;
	bool intersect(const Box3& b) const;
};