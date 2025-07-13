#include "quad_tree_utils.h"
using namespace std;
using namespace chrono;

void CreateRandomPoint(QuadTree& tree) {
	auto start = high_resolution_clock::now();
	const int size = 50;
	vector<Point> all_points;
	for (int i = 0; i < size; i++) {
		int dir[] = { -1, 1 };
		Point p;
		p.x = Random(0, win_width - 1);
		p.y = Random(0, win_height - 1);
		p.vx = Random(25, 50) * dir[Random(0, 1)];
		p.vy = Random(25, 50) * dir[Random(0, 1)];
		tree.insert(p);

		all_points.push_back(p);
	}

	auto end = high_resolution_clock::now();
	cout << "Квадродерево создалось за: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
}
void Test(const QuadTree& tree) {
	static double rl = 0;
	SDL_Rect rect;
	static Box find_box(0, 0, 0, 0);
	vector<Point> res_find;

	rl -= fps.dt;
	if (rl <= 0) {
		rl = 5;
		cout << "1) Размер дерева: " << tree.getSize() << "\n" << endl;
		find_box = Box(Random(0, win_width - 1),
			Random(0, win_height - 1),
			Random(100, 400),
			Random(100, 400));

		cout << "2) Ищем точки в боксе: " << find_box.x << ", " << find_box.y << ", " << find_box.w << ", " << find_box.h << endl;
		res_find = tree.queryRange(find_box);
		cout << "Найденные точки: " << endl;
		for (int i = 0; i < res_find.size(); ++i) {
			cout << "Точка " << i + 1 << ": " << res_find[i].x << ", " << res_find[i].y << endl;
		}
		cout << endl;

		double radius = Random(1, 100);
		cout << "3) Ищем пары точек, которые находятся друг от друга в радиусе: " << radius << endl;
		vector<pair<Point, Point>> res_pair = tree.findAllPairsInRadius(radius);
		cout << "Найденные пары: " << endl;
		for (const auto& pair : res_pair) {
			cout << "(" << pair.first.x << ", " << pair.first.y << ") <-> ("
				<< pair.second.x << ", " << pair.second.y << ")" << endl;
		}

		cout << "--------------------------" << endl;
	}

	// Вывод информации о поиске
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	rect = { (int)find_box.x, (int)find_box.y, (int)find_box.w, (int)find_box.h };
	SDL_RenderDrawRect(ren, &rect);
}