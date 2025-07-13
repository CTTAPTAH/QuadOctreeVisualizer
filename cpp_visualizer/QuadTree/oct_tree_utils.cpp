#include "oct_tree_utils.h"
using namespace std;
using namespace chrono;

void CreateRandomPointOct(OctTree& tree) {
	auto start = high_resolution_clock::now();
	const int size = 3;
	for (int i = 0; i < size; i++) {
		int dir[] = { -1, 1 };
		Point3 p;
		p.x = Random(0, 512 - 1);
		p.y = Random(0, 512 - 1);
		p.z = Random(0, 512 - 1);
		p.vx = Random(25, 50) * dir[Random(0, 1)];
		p.vy = Random(25, 50) * dir[Random(0, 1)];
		p.vz = Random(25, 50) * dir[Random(0, 1)];
		tree.insert(p);
	}

	auto end = high_resolution_clock::now();
	cout << "Октодерево создалось за: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

}
void TestOct(const OctTree& tree) {
	static double rl = 0;
	static Box3 find_box(0, 0, 0, 0, 0, 0); // 3D область поиска
	vector<Point3> res_find;

	rl -= fps.dt;
	if (rl <= 0) {
		rl = 5;
		cout << "1) Размер дерева: " << tree.getSize() << "\n" << endl;

		find_box = Box3(
			Random(0, 512 - 1),
			Random(0, 512 - 1),
			Random(0, 512 - 1),
			Random(100, 512),
			Random(100, 512),
			Random(100, 512));

		cout << "2) Ищем точки в боксе: " << find_box.x << ", " << find_box.y << ", " << find_box.z
			<< ", " << find_box.w << ", " << find_box.h << ", " << find_box.d << endl;

		res_find = tree.queryRange(find_box);

		cout << "Найденные точки: " << endl;
		for (int i = 0; i < res_find.size(); ++i) {
			cout << "Точка " << i + 1 << ": "
				<< res_find[i].x << ", " << res_find[i].y << ", " << res_find[i].z << endl;
		}
		cout << endl;

		double radius = Random(1, 100);
		cout << "3) Ищем пары точек, которые находятся друг от друга в радиусе: " << radius << endl;
		vector<pair<Point3, Point3>> res_pair = tree.findAllPairsInRadius(radius);
		cout << "Найденные пары: " << endl;
		for (const auto& pair : res_pair) {
			cout << "(" << pair.first.x << ", " << pair.first.y << ", " << pair.first.z << ") <-> ("
				<< pair.second.x << ", " << pair.second.y << ", " << pair.second.z << ")" << endl;
		}
		cout << "--------------------------" << endl;
	}

	// Визуализируем 2D проекцию области поиска (только x и y)
	draw3DBox(find_box, { 255, 255, 255, 255 });
	//drawFilledBox(find_box, ren, 255, 255, 255, 50);
}

// Класс для визуализации октантов
VisualOct::VisualOct() : idx_color(0)
{
	// создаём цвета
	for (int i = 0; i < 12; ++i) {
		float t = i / 12.0f;
		Uint8 r = (Uint8)(sin(2 * M_PI * t + 0) * 127 + 128);
		Uint8 g = (Uint8)(sin(2 * M_PI * t + 2) * 127 + 128);
		Uint8 b = (Uint8)(sin(2 * M_PI * t + 4) * 127 + 128);
		// Исключаем явно "зелёные" цвета
		if (g > r + 40 and g > b + 40) continue;

		SDL_Color c = { r, g, b, 255 };
		palette.push_back(c);
	}
	// перемешиваем их
	int number_mixes = Random(100, 1000);
	if (!palette.empty()) {
		for (int i = 30; i < number_mixes; ++i) {
			int a = Random(0, palette.size() - 1);
			int b = Random(0, palette.size() - 1);
			swap(palette[a], palette[b]);
		}
	}
}

void VisualOct::addBoxes(const vector<Box3>& boxes) {
	for (int i = 0; i < boxes.size(); ++i) {
		addBox(boxes[i]);
	}
}
void VisualOct::addBox(const Box3& b) {
	visualNodes.push_back({ b, palette[idx_color] });
	++idx_color;
	if (idx_color >= palette.size()) {
		idx_color = 0;
	}
}
void VisualOct::removeBox(const Box3& b) {
	for (auto it = visualNodes.begin(); it != visualNodes.end();) {
		if (it->first.x == b.x and it->first.y == b.y and it->first.z == b.z) {
			visualNodes.erase(it);
		}
		else {
			++it;
		}
	}
}

void VisualOct::draw() {
	for (const auto& p : visualNodes) {
		// нарисуем прямоугольник проекции Box3 на XY (игнорируем Z)
		SDL_Color c = p.second;

		drawFilledBox(p.first, ren, c.r, c.g, c.b, 50); // полупрозрачный синий
		draw3DBox(p.first, { c.r, c.g, c.b, 255 });
	}
}