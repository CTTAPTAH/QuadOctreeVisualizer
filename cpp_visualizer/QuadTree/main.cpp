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
	// ����� ������ ��� ������������
	int choice = 0; // �� ��������� ������������. 0 - ������, 1 - ����
	if (argc > 1)
		if (string(argv[1]) == "1") // ���� �������� 1 � �������� ����������
			choice = 1; // ����������


	srand(time(nullptr));
	system("chcp 1251>nul");
	Init();
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	bool isRunning = true;  SDL_Event ev;

	// �������� ������
	QuadTree tree({ 0, 0, (double)win_width, (double)win_height });
	OctTree oct_tree({ 0, 0, 0, 512, 512, 512 });

	// ��������� �����
	if (!choice) CreateRandomPoint(tree);
	else CreateRandomPointOct(oct_tree);

	// ��� ����������� ���������� ����� � ����������
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
		// �������� ����� (������ ������ ������)
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); // ����: ������
		SDL_RenderClear(ren); // ������� �����

		if (!choice) {
			// ���������  ������������
			tree.updatePoints(fps.dt);
			tree.draw();
			Test(tree);
		}
		else {
			// ����������
			oct_tree.draw();
			vs_oct.draw();
			TestOct(oct_tree);
		}

		SDL_RenderPresent(ren);
	}

	DeInit(0);

	return 0;
}