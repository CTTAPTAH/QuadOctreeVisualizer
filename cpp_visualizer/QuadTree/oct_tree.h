#pragma once
#include <iostream>
#include <vector>
#include "globals.h"
#include "geometry3d.h"
#include "draw3d.h"

class OctTree {
private:
	struct Node {
		Box3 box;
		std::vector<Point3> points;
		Node* children[8] = { nullptr };

		Node(Box3 b) : box(b) {}
	};
	Node* root;       // корневой узел
	int threshold; // максимальное количество точек в узле
	int depth; 

	// методы
	int findChildIdx(const Box3& box, const Point3& p);
	void divide(Node* node, int cur_depth);
	void insert(Node* node, const Point3& point, int depth = 0);
	void remove(Node* node, const Point3& p);
	void queryRange(Node* node, const Box3& range, std::vector<Point3>& result) const;
	void findAllPairsInRadius(Node* node, double radius,
		std::vector<std::pair<Point3, Point3>>& result) const;
	// Вывод информации
	void draw(Node* node) const;

	// очитска дерева
	void clear(Node* node);

	// геттеры, сеттеры
	void getAllNotEmptyBox(Node* node, std::vector<Box3>& res) const;
	int getSize(Node* node) const;
public:
	OctTree(Box3 box);
	~OctTree();

	// методы
	void insert(const Point3& point);
	void remove(const Point3& p);
	std::vector<Point3> queryRange(const Box3& range) const;
	std::vector<std::pair<Point3, Point3>> findAllPairsInRadius(const double& radius) const;

	// Вывод информации
	void draw() const;

	// очитска дерева
	void clear();

	// геттеры, сеттеры
	std::vector<Box3> getAllNotEmptyBox() const;
	int getSize() const;
};