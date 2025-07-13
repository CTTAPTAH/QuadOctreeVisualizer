#include <iostream>
#include <vector>
#include <chrono>
#include <SDL.h>
#include "quad_tree_utils.h"
#include "oct_tree_utils.h"
using namespace std;
using namespace chrono;

SDL_Window* win = nullptr;
SDL_Renderer* ren = nullptr;
int win_width = 1024, win_height = 800;
Dt fps;


int main(int argc, char* argv[])
{
	// выбор дерева для демонстрации
	int choice = 0; // По умолчанию квадродерево. 0 - квадро, 1 - окто
	if (argc > 1)
		if (string(argv[1]) == "1") // Если передали 1 — включить октодерево
			choice = 1; // Октодерево


	srand(time(nullptr));
	system("chcp 1251>nul");
	Init();
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	bool isRunning = true;  SDL_Event ev;

	// Создание дерева
	QuadTree tree({ 0, 0, (double)win_width, (double)win_height });
	OctTree oct_tree({ 0, 0, 0, 512, 512, 512 });

	// добавляем точки
	if (!choice) CreateRandomPoint(tree);
	else CreateRandomPointOct(oct_tree);

	// для наглядности нахождения точки в октодереве
	VisualOct vs_oct;
	vs_oct.addBoxes(oct_tree.getAllNotEmptyBox());

	while (isRunning)
	{
		LimitFPS();

		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			}
		}
		// Очистить экран (залить чёрным цветом)
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); // Цвет: чёрный
		SDL_RenderClear(ren); // Очищаем экран

		if (!choice) {
			// Обновляем  квадродерево
			tree.updatePoints(fps.dt);
			tree.draw();
			Test(tree);
		}
		else {
			// Октодерево
			oct_tree.draw();
			vs_oct.draw();
			TestOct(oct_tree);
		}

		SDL_RenderPresent(ren);
	}

	DeInit(0);

	return 0;
}