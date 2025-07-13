#include "oct_tree.h"
using namespace std;

// ����������
//-------------------------------------------------
// ��������� ����
// ������
int OctTree::findChildIdx(const Box3& box, const Point3& p) {
	int idx = 0;
	if (p.x > box.getCenterX()) idx |= 1;
	if (p.y > box.getCenterY()) idx |= 2;
	if (p.z > box.getCenterZ()) idx |= 4;
	return idx;
}
void OctTree::divide(Node* node, int cur_depth) {
	double x = node->box.x;
	double y = node->box.y;
	double z = node->box.z;
	double halfW = node->box.getHalfW();
	double halfH = node->box.getHalfH();
	double halfD = node->box.getHalfD();
	if (halfW <= 1 or halfH <= 1 or halfD <= 1 or cur_depth >= depth) return; // ����������� ������ ���������

	// ������ �������� ��� ����������
	for (int i = 0; i < 8; i++) {
		double newX = x + (i & 1) * halfW;
		double newY = y + ((i >> 1) & 1) * halfH;
		double newZ = z + ((i >> 2) & 1) * halfD;

		node->children[i] = new Node({ newX, newY, newZ, halfW, halfH, halfD });
	}

	// ���������� ����� �� ������ ���������
	for (const Point3& p : node->points) {
		int idx = findChildIdx(node->box, p);
		insert(node->children[idx], p);
	}
	node->points.clear(); // �������, �.�. ������ ����� ������ � �������.
}
void OctTree::insert(Node* node, const Point3& point, int depth) {
	// ���� ����� ��������� ��� �����, �� �� ���������
	if (!node->box.contains(point)) return;

	//���� ���� �� ����� ��������, ��������� ����� � ����.���� ����� ���������� ������ �����������
	//�����, ����� ���� �� 4 �������� � ��������� ��� ��� ����� � �������� ����.
	//��������� ������ �� ������������� ����� �����, ���� ���� ��� ����, �� ������ ���� �����
	if (!node->children[0]) {
		node->points.push_back(point);

		if (node->points.size() > threshold) {
			divide(node, depth + 1);
		}
		return;
	}

	// ���� ����, ���� ������� �����
	int idx = findChildIdx(node->box, point);
	insert(node->children[idx], point, depth + 1);
}
void OctTree::remove(Node* node, const Point3& p) {
	if (!node->box.contains(p)) return;

	// ���� ��� ���� � ���� � ������� �����
	if (node->children[0] == nullptr) {
		for (auto it = node->points.begin(); it != node->points.end(); ++it) {
			if (it->x == p.x and it->y == p.y and it->z == p.z) {
				node->points.erase(it);
				return;
			}
		}
		return;
	}

	// ����������� � ������� ������
	int idx = findChildIdx(node->box, p);
	remove(node->children[idx], p);

	// ��������, ����� �� ��������� �����
	vector<Point3> allPoints;
	int totalPoints = 0;

	for (int i = 0; i < 8; ++i) {
		Node* c = node->children[i];
		if (!c) continue;

		if (c->children[0]) {
			// � ������ ���� ���� ���� - ������ ����������
			return;
		}

		totalPoints += c->points.size();
		allPoints.insert(allPoints.end(), c->points.begin(), c->points.end());
	}

	// ���� ����� ����� ����� � ����� <= threshold
	if (totalPoints <= threshold) {
		// �������� ��� ����� � ������������ ����
		node->points = move(allPoints);

		// ������� ���� �����
		for (int i = 0; i < 8; ++i) {
			delete node->children[i];
			node->children[i] = nullptr;
		}
	}
}
void OctTree::queryRange(Node* node, const Box3& range, vector<Point3>& result) const {
	if (!node->box.intersect(range)) return;

	// �������� ����� �������� ����
	for (const Point3& p : node->points) {
		if (range.contains(p)) {
			result.push_back(p);
		}
	}

	// ���������� ��������� ���� �����
	for (int i = 0; i < 8; ++i) {
		if (node->children[i]) {
			queryRange(node->children[i], range, result);
		}
	}
}
void OctTree::findAllPairsInRadius(Node* node, double radius, vector<pair<Point3, Point3>>& result) const {
	if (!node) return;

	// ��� ������ ����� ���� ������ ����� ����������
	for (const Point3& p : node->points) {
		Box3 range(
			p.x - radius / 2, p.y - radius / 2, p.z - radius / 2,
			radius, radius, radius
		);

		vector<Point3> nearby;
		queryRange(root, range, nearby);

		// ��������� ����� ���������, ����� �� ���� ��������
		for (const Point3& other : nearby) {
			if (p.x == other.x and p.y == other.y and p.z == other.z) continue; // ��������� ���� ����
			// �������� ��������
			if (p.x < other.x or (p.x == other.x and p.y < other.y) or
				(p.x == other.x and p.y == other.y and p.z < other.z)) {
				result.emplace_back(p, other);
			}
		}
	}

	// ����������� ����� �����
	for (int i = 0; i < 8; ++i) {
		if (node->children[i]) {
			findAllPairsInRadius(node->children[i], radius, result);
		}
	}
}
// ����� ����������
void OctTree::draw(Node* node) const {
	if (!node) return;

	draw3DBox(node->box, { 0, 255, 0, 255 });

	// �������� ����� (���� ����)
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // �������
	for (const Point3& p : node->points) {
		int screenX = projectX(p.x, p.z);
		int screenY = projectY(p.y, p.z);
		SDL_Rect pt = { screenX, screenY, 4, 4 };
		SDL_RenderFillRect(ren, &pt);
	}

	// ���������� �������� ��������
	for (int i = 0; i < 8; ++i) {
		draw(node->children[i]);
	}
}

// ������� ������
void OctTree::clear(Node* node) {
	if (!node) return;

	for (int i = 0; i < 8; ++i) {
		clear(node->children[i]);
	}

	delete node;
}

// �������, �������
void OctTree::getAllNotEmptyBox(Node* node, vector<Box3>& res) const {
	if (!node) return;

	if (!node->points.empty()) {
		res.push_back(node->box);
	}

	// ���������� �������� ��������
	for (int i = 0; i < 8; ++i) {
		getAllNotEmptyBox(node->children[i], res);
	}
}
int OctTree::getSize(Node* node) const {
	if (!node) return 0;

	int total = node->points.size(); // ������� �����

	for (int i = 0; i < 8; ++i) {
		total += getSize(node->children[i]); // ���������� ������� �����
	}

	return total;
}
//-------------------------------------------------
// ��������� ����
OctTree::OctTree(Box3 box) : threshold(1), depth(6) {
	root = new Node(box);
}
OctTree::~OctTree() {
	clear(root);
}

// ������
void OctTree::insert(const Point3& point) {
	insert(root, point);
}
void OctTree::remove(const Point3& p) {
	remove(root, p);
}
vector<Point3> OctTree::queryRange(const Box3& range) const {
	vector<Point3> res;
	queryRange(root, range, res);
	return res;
}
vector<pair<Point3, Point3>> OctTree::findAllPairsInRadius(const double& radius) const {
	vector<pair<Point3, Point3>> res;
	findAllPairsInRadius(root, radius, res);
	return res;
}

// ����� ����������
void OctTree::draw() const {
	//draw3DBox(root->box, { 0, 255, 0, 255 });
	draw(root);
}

// ������� ������
void OctTree::clear() {
	clear(root);
}
 
// �������, �������
vector<Box3> OctTree::getAllNotEmptyBox() const {
	vector<Box3> res;
	getAllNotEmptyBox(root, res);
	return res;
}
int OctTree::getSize() const {
	return getSize(root);
}
//-------------------------------------------------