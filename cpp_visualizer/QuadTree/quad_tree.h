#pragma once
#include <iostream>
#include <vector>
#include "globals.h"

struct Point {
	double x = 0, y = 0;
	double vx = 1, vy = 1;

	Point();
	Point(double x_, double y_);
	bool operator==(const Point& p);
};
struct Box {
	double x = 0, y = 0;
	double w = 0, h = 0;

	Box(double x_, double y_, double w_, double h_);
	float getCenterX() const;
	float getCenterY() const;
	float getHalfW() const;
	float getHalfH() const;
	bool contains(const Point& p) const;
	bool intersect(const Box& b) const;
};
class QuadTree {
private:
	struct Node {
		Box box;
		std::vector<Point> points;
		Node* topLeft = nullptr;
		Node* topRight = nullptr;
		Node* botLeft = nullptr;
		Node* botRight = nullptr;

		Node(Box b) : box(b) {}
	};
	Node* root;
	int depth; // ������������ ������� ���� (������ ������)
	int threshold; // ������������ ���������� ��������, ������� ����� ��������� ����
	// ��������: ������ �� ���� (��� ����� � ��� ��������)

	// ������
	bool isEmpty(Node* node);
	void updatePoints(Node* node, const double& dt,
		std::vector<std::pair<Point, Point>>& pointsToMove);
	Node* findChild(Node* node, const Point& p);
	void divide(Node* node, int cur_depth);
	void insert(Node* node, const Point& point, int depth = 0);
	void remove(Node* node, const Point& point);
	void queryRange(Node* node, const Box& range, std::vector<Point>& result) const;
	void findAllPairsInRadius(Node* node, double radius,
		std::vector<std::pair<Point, Point>>& result) const;
	// ����� �������� ������
	void print(Node* node) const;
	void draw(Node* node) const;
	// ������� ������
	void clear(Node* node);
	// ������� �������
	int getSize(Node* node) const;
public:
	// �����������, ����������
	QuadTree(Box box);
	~QuadTree();

	// ������
	void insert(const Point& point);
	void remove(const Point& point);
	std::vector<Point> queryRange(const Box& range) const;
	std::vector<std::pair<Point, Point>> findAllPairsInRadius(double radius) const;
	// �������� �����
	void updatePoints(const double& dt);
	// ����� �������� ������
	void print() const;
	void draw() const;
	// ������� ������
	void clear();
	// �������, �������
	Node* getRoot();
	int getSize() const;
};