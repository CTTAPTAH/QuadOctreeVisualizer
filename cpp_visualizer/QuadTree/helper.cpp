#include "helper.h"

void DeInit(int error)
{
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(error);
}
void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf_s("�� ������� ��������������� SDL. ������: %s", SDL_GetError());
		DeInit(1);
	}
	win = SDL_CreateWindow("Tree", SDL_WINDOWPOS_CENTERED, 25, win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_MAXIMIZED*/);
	if (win == 0) {
		printf_s("�� ������� ������� ����. ������: %s", SDL_GetError());
		DeInit(1);
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == 0) {
		printf_s("�� ������� ������� �����������. ������: %s", SDL_GetError());
		DeInit(1);
	}
}
int Random(int start, int end) {
	return rand() % (end - start + 1) + start;
}
void LimitFPS()
{
	fps.newtime = SDL_GetTicks() / 1000.0;
	fps.dt = fps.newtime - fps.lasttime; // ����� ���� �������� ����� �������
	const double target_frame_time = 1.0 / fps.fps; // � �������� ������� ������ ����
	if (fps.dt < target_frame_time) {
		SDL_Delay((Uint32)((target_frame_time - fps.dt) * 1000.0)); // ��������, ����� fps ��� ����� ��������� �
		fps.newtime = SDL_GetTicks() / 1000.0;
		fps.dt = fps.newtime - fps.lasttime; // ����� ���� �������� ����� ������� + ��������
	}
	fps.lasttime = fps.newtime;

}