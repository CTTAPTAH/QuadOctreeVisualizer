#include "quad_tree.h"
using namespace std;

// Точка
Point::Point() {
	x = y = 0;
	vx = vy = 100;
}
Point::Point(double x_, double y_) {
	x = x_;
	y = y_;
	vx = vy = 100;
}
bool Point::operator==(const Point& p) {
	return x == p.x and y == p.y;
}

// Бокс
Box::Box(double x_, double y_, double w_, double h_) {
	x = x_;
	y = y_;
	w = w_;
	h = h_;
}
float Box::getCenterX() const { return x + w / 2.0f; }
float Box::getCenterY() const { return y + h / 2.0f; }
float Box::getHalfW() const { return w / 2.0f; }
float Box::getHalfH() const { return h / 2.0f; }
bool Box::contains(const Point& p) const {
	return (p.x >= x) and (p.x < x + w) and
		(p.y >= y) and (p.y < y + h);
}
bool Box::intersect(const Box& b) const {
	return !(x + w < b.x or b.x + b.w < x or
		y + h < b.y or b.y + b.h < y);
}

// Квадродерево
//-------------------------------------------------
// Приватное поле
bool QuadTree::isEmpty(Node* node) {
	return node and node->points.empty() and !node->topLeft;
}
void QuadTree::updatePoints(Node* node, const double& dt,
	vector<pair<Point, Point>>& pointsToMove) {
	if (!node->topLeft) {
		for (auto& p : node->points) {
			Point old = p; // Сохраняем старые координаты

			// Обновляем положение
			p.x += p.vx * dt;
			p.y += p.vy * dt;

			// Отражение от стен
			if (p.x <= 0 or p.x >= win_width) {
				p.vx = -p.vx;
				p.x = max(0.0 + 1, min((double)win_width, p.x) - 1);
			}
			if (p.y <= 0 or p.y >= win_height) {
				p.vy = -p.vy;
				p.y = max(0.0 + 1, min((double)win_height, p.y) - 1);
			}

			// Если точка вышла за пределы своего квадрата
			if (!node->box.contains(p)) {
				pointsToMove.push_back({ old, p });
				p = old;
			}
		}
		return;
	}

	updatePoints(node->topLeft, dt, pointsToMove);
	updatePoints(node->topRight, dt, pointsToMove);
	updatePoints(node->botLeft, dt, pointsToMove);
	updatePoints(node->botRight, dt, pointsToMove);
}
QuadTree::Node* QuadTree::findChild(Node* node, const Point& p) {
	bool left = p.x < node->box.getCenterX();
	bool top = p.y < node->box.getCenterY();

	if (top) {
		return left ? node->topLeft : node->topRight;
	}
	else {
		return left ? node->botLeft : node->botRight;
	}
}
void QuadTree::divide(Node* node, int cur_depth) {
	double x = node->box.x;
	double y = node->box.y;
	double halfW = node->box.getHalfW();
	double halfH = node->box.getHalfH();
	if (halfW <= 1 or halfH <= 1 or cur_depth >= depth) return; // минимальный размер квадранта

	// создаём квадраты для разделения
	node->topLeft = new Node({ x, y, halfW , halfH });
	node->topRight = new Node({ x + halfW, y, halfW , halfH });
	node->botLeft = new Node({ x, y + halfH, halfW , halfH });
	node->botRight = new Node({ x + halfW, y + halfH, halfW , halfH });

	// перемещаем точки по нужным квадратам
	for (const Point& p : node->points) {
		insert(findChild(node, p), p, cur_depth + 1);
	}
	node->points.clear(); // очищаем, т.к. храним точки только в листьях.
}
void QuadTree::insert(Node* node, const Point& point, int depth) {
	// если точка находится вне бокса, то не добавляем
	if (!node->box.contains(point)) return;

	//Если узел не имеет потомков, добавляем точку в него.Если точек становится больше допустимого
	//числа, делим узел на 4 дочерних и переносим все его точки в дочерние узлы.
	//Проверяем только на существование одной ветви, ведь если она есть, то другие тоже будут
	if (!node->topLeft) {
		node->points.push_back(point);

		if (node->points.size() > threshold) {
			divide(node, depth + 1);
		}
		return;
	}

	// ищем узел, куда добавим точку
	insert(findChild(node, point), point, depth + 1);
}
void QuadTree::remove(Node* node, const Point& point) {
	// если точка находится вне бокса, то не добавляем
	if (!node->box.contains(point)) return;

	// если это лист, то удаляем точку
	if (!node->topLeft) {
		for (auto it = node->points.begin(); it != node->points.end(); ++it) {
			if (point.x == it->x and point.y == it->y) {
				node->points.erase(it);
				return;
			}
		}
		return;
	}

	remove(findChild(node, point), point);

	// После удаления проверяем, можно ли схлопнуть детей
	int totalPoints = 0;
	vector<Point> allPoints;
	Node* children[4] = { node->topLeft, node->topRight, node->botLeft, node->botRight };

	// Считаем все точки у детей
	for (Node* child : children) {
		if (child->topLeft) {
			// У ребёнка есть свои дети — нельзя схлопывать
			return;
		}
		totalPoints += child->points.size();
		allPoints.insert(allPoints.end(), child->points.begin(), child->points.end());
	}

	// Если общее число точек у детей <= threshold
	if (totalPoints <= threshold) {
		// Собираем все точки в родительский узел
		node->points = move(allPoints);

		// Удаляем всех детей
		for (Node* child : children) {
			delete child;
		}
		node->topLeft = node->topRight = node->botLeft = node->botRight = nullptr;
	}
}
void QuadTree::queryRange(Node* node, const Box& range,
	vector<Point>& result) const {
	if (!node->box.intersect(range)) return;

	// Проверка точек в этом узле
	for (int i = 0; i < node->points.size(); ++i) {
		if (range.contains(node->points[i])) {
			result.push_back(node->points[i]);
		}
	}

	// Рекурсивно проверяем детей
	if (node->topLeft) queryRange(node->topLeft, range, result);
	if (node->topRight) queryRange(node->topRight, range, result);
	if (node->botLeft) queryRange(node->botLeft, range, result);
	if (node->botRight) queryRange(node->botRight, range, result);
}
void QuadTree::findAllPairsInRadius(Node* node, double radius,
	vector<pair<Point, Point>>& result) const {
	if (!node) return;

	// Для каждой точки ищем другие точки поблизости
	for (const Point& p : node->points) {
		Box range(p.x - radius / 2, p.y - radius / 2, radius, radius);
		vector<Point> nearby;
		queryRange(root, range, nearby);
		// найденные точки фильтруем, чтобы не было повторов
		for (const Point& other : nearby) {
			if (p.x == other.x and p.y == other.y) continue; // исключаем саму себя
			if (p.x < other.x or (p.x == other.x and p.y < other.y)) // избегаем повторов
				result.emplace_back(p, other);
		}
	}

	// Рекурсивно обходим детей
	findAllPairsInRadius(node->topLeft, radius, result);
	findAllPairsInRadius(node->topRight, radius, result);
	findAllPairsInRadius(node->botLeft, radius, result);
	findAllPairsInRadius(node->botRight, radius, result);
}
// Вывод значений дерева
void QuadTree::print(Node* node) const {
	if (!node) {
		return;
	}

	if (!node->points.empty()) {
		for (int i = 0; i < node->points.size(); i++) {
			cout << node->points[i].x << ":" << node->points[i].y << " -> ";
		}
		cout << endl;
	}
	print(node->topLeft);
	print(node->botLeft);
	print(node->topRight);
	print(node->botRight);
}
void QuadTree::draw(Node* node) const {
	if (!node) return;

	if (!node->points.empty()) {
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		for (int i = 0; i < node->points.size(); i++) {
			SDL_Rect rect = { node->points[i].x, node->points[i].y, 4, 4 };
			SDL_RenderFillRect(ren, &rect);
		}
	}
	SDL_Rect rect{ node->box.x, node->box.y , node->box.w , node->box.h };
	SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
	SDL_RenderDrawRect(ren, &rect);

	draw(node->topLeft);
	draw(node->botLeft);
	draw(node->topRight);
	draw(node->botRight);
}
// очистка дерева
void QuadTree::clear(Node* node) {
	if (!node) return;
	clear(node->topLeft);
	clear(node->topRight);
	clear(node->botLeft);
	clear(node->botRight);
	delete node;
}
// геттеры сеттеры
int QuadTree::getSize(Node* node) const {
	if (!node) return 0;

	int total = node->points.size(); // если есть точки, прибавляем их
	total += getSize(node->topLeft);
	total += getSize(node->topRight);
	total += getSize(node->botLeft);
	total += getSize(node->botRight);

	return total;
}
//-------------------------------------------------
// Публичное поле
// конструктор, деструктор
QuadTree::QuadTree(Box box) : depth(7), threshold(1) {
	root = new Node(box);
}
QuadTree::~QuadTree() {
	clear(root);
}

// методы
void QuadTree::insert(const Point& point) {
	insert(root, point);
}
void QuadTree::remove(const Point& point) {
	remove(root, point);
}
vector<Point> QuadTree::queryRange(const Box& range) const {
	vector<Point> result;
	queryRange(root, range, result);
	return result;
}
vector<pair<Point, Point>> QuadTree::findAllPairsInRadius(double radius) const {
	vector<pair<Point, Point>> res;
	findAllPairsInRadius(root, radius, res);
	return res;
}
// движение точек
void QuadTree::updatePoints(const double& dt) {
	vector<pair<Point, Point>> pointsToMove;
	updatePoints(root, dt, pointsToMove);

	for (const auto& p : pointsToMove) {
		remove(root, p.first);
		insert(root, p.second);
	}
}
// Вывод значений дерева
void QuadTree::print() const {
	print(root);
}
void QuadTree::draw() const {
	draw(root);
}
// очистка дерева
void QuadTree::clear() {
	clear(root);
}
// Геттеры, сеттеры
QuadTree::Node* QuadTree::getRoot() {
	return root;
}
int QuadTree::getSize() const {
	return getSize(root);
}
//-------------------------------------------------