#include "oct_tree.h"
using namespace std;

// Октодерево
//-------------------------------------------------
// Приватное поле
// методы
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
	if (halfW <= 1 or halfH <= 1 or halfD <= 1 or cur_depth >= depth) return; // минимальный размер квадранта

	// создаём квадраты для разделения
	for (int i = 0; i < 8; i++) {
		double newX = x + (i & 1) * halfW;
		double newY = y + ((i >> 1) & 1) * halfH;
		double newZ = z + ((i >> 2) & 1) * halfD;

		node->children[i] = new Node({ newX, newY, newZ, halfW, halfH, halfD });
	}

	// перемещаем точки по нужным квадратам
	for (const Point3& p : node->points) {
		int idx = findChildIdx(node->box, p);
		insert(node->children[idx], p);
	}
	node->points.clear(); // очищаем, т.к. храним точки только в листьях.
}
void OctTree::insert(Node* node, const Point3& point, int depth) {
	// если точка находится вне бокса, то не добавляем
	if (!node->box.contains(point)) return;

	//Если узел не имеет потомков, добавляем точку в него.Если точек становится больше допустимого
	//числа, делим узел на 4 дочерних и переносим все его точки в дочерние узлы.
	//Проверяем только на существование одной ветви, ведь если она есть, то другие тоже будут
	if (!node->children[0]) {
		node->points.push_back(point);

		if (node->points.size() > threshold) {
			divide(node, depth + 1);
		}
		return;
	}

	// ищем узел, куда добавим точку
	int idx = findChildIdx(node->box, point);
	insert(node->children[idx], point, depth + 1);
}
void OctTree::remove(Node* node, const Point3& p) {
	if (!node->box.contains(p)) return;

	// Если это лист — ищем и удаляем точку
	if (node->children[0] == nullptr) {
		for (auto it = node->points.begin(); it != node->points.end(); ++it) {
			if (it->x == p.x and it->y == p.y and it->z == p.z) {
				node->points.erase(it);
				return;
			}
		}
		return;
	}

	// Углубляемся в нужного ребёнка
	int idx = findChildIdx(node->box, p);
	remove(node->children[idx], p);

	// Проверка, можно ли схлопнуть детей
	vector<Point3> allPoints;
	int totalPoints = 0;

	for (int i = 0; i < 8; ++i) {
		Node* c = node->children[i];
		if (!c) continue;

		if (c->children[0]) {
			// у ребёнка есть свои дети - нельзя схлопывать
			return;
		}

		totalPoints += c->points.size();
		allPoints.insert(allPoints.end(), c->points.begin(), c->points.end());
	}

	// Если общее число точек у детей <= threshold
	if (totalPoints <= threshold) {
		// Собираем все точки в родительский узел
		node->points = move(allPoints);

		// Удаляем всех детей
		for (int i = 0; i < 8; ++i) {
			delete node->children[i];
			node->children[i] = nullptr;
		}
	}
}
void OctTree::queryRange(Node* node, const Box3& range, vector<Point3>& result) const {
	if (!node->box.intersect(range)) return;

	// Проверка точек текущего узла
	for (const Point3& p : node->points) {
		if (range.contains(p)) {
			result.push_back(p);
		}
	}

	// Рекурсивно проверяем всех детей
	for (int i = 0; i < 8; ++i) {
		if (node->children[i]) {
			queryRange(node->children[i], range, result);
		}
	}
}
void OctTree::findAllPairsInRadius(Node* node, double radius, vector<pair<Point3, Point3>>& result) const {
	if (!node) return;

	// Для каждой точки ищем другие точки поблизости
	for (const Point3& p : node->points) {
		Box3 range(
			p.x - radius / 2, p.y - radius / 2, p.z - radius / 2,
			radius, radius, radius
		);

		vector<Point3> nearby;
		queryRange(root, range, nearby);

		// найденные точки фильтруем, чтобы не было повторов
		for (const Point3& other : nearby) {
			if (p.x == other.x and p.y == other.y and p.z == other.z) continue; // исключаем саму себя
			// избегаем повторов
			if (p.x < other.x or (p.x == other.x and p.y < other.y) or
				(p.x == other.x and p.y == other.y and p.z < other.z)) {
				result.emplace_back(p, other);
			}
		}
	}

	// Рекурсивный обход детей
	for (int i = 0; i < 8; ++i) {
		if (node->children[i]) {
			findAllPairsInRadius(node->children[i], radius, result);
		}
	}
}
// Вывод информации
void OctTree::draw(Node* node) const {
	if (!node) return;

	draw3DBox(node->box, { 0, 255, 0, 255 });

	// нарисуем точки (если есть)
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // красный
	for (const Point3& p : node->points) {
		int screenX = projectX(p.x, p.z);
		int screenY = projectY(p.y, p.z);
		SDL_Rect pt = { screenX, screenY, 4, 4 };
		SDL_RenderFillRect(ren, &pt);
	}

	// рекурсивно нарисуем потомков
	for (int i = 0; i < 8; ++i) {
		draw(node->children[i]);
	}
}

// очитска дерева
void OctTree::clear(Node* node) {
	if (!node) return;

	for (int i = 0; i < 8; ++i) {
		clear(node->children[i]);
	}

	delete node;
}

// геттеры, сеттеры
void OctTree::getAllNotEmptyBox(Node* node, vector<Box3>& res) const {
	if (!node) return;

	if (!node->points.empty()) {
		res.push_back(node->box);
	}

	// рекурсивно нарисуем потомков
	for (int i = 0; i < 8; ++i) {
		getAllNotEmptyBox(node->children[i], res);
	}
}
int OctTree::getSize(Node* node) const {
	if (!node) return 0;

	int total = node->points.size(); // текущие точки

	for (int i = 0; i < 8; ++i) {
		total += getSize(node->children[i]); // рекурсивно обходим детей
	}

	return total;
}
//-------------------------------------------------
// Публичное поле
OctTree::OctTree(Box3 box) : threshold(1), depth(6) {
	root = new Node(box);
}
OctTree::~OctTree() {
	clear(root);
}

// методы
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

// Вывод информации
void OctTree::draw() const {
	//draw3DBox(root->box, { 0, 255, 0, 255 });
	draw(root);
}

// очитска дерева
void OctTree::clear() {
	clear(root);
}
 
// геттеры, сеттеры
vector<Box3> OctTree::getAllNotEmptyBox() const {
	vector<Box3> res;
	getAllNotEmptyBox(root, res);
	return res;
}
int OctTree::getSize() const {
	return getSize(root);
}
//-------------------------------------------------