#include "draw3d.h"

int projectX(double x, double z) {
	// static_cast<int>(x - z * cos(45.0 * M_PI / 180.0)); // наклон по диагонали
	const double scale = 0.7; // уменьшаем всЄ на 80%
	int offsetX = 2 * win_width / 5;
	return static_cast<int>((x - z * 0.5) * scale) + offsetX;
}
int projectY(double y, double z) {
	// return static_cast<int>(y - z * sin(45.0 * M_PI / 180.0));
	const double scale = 0.7; // уменьшаем всЄ на 80%
	int offsetY = 2 * win_height / 5;
	return static_cast<int>((y - z * 0.5) * scale) + offsetY;
}
void draw3DBox(const Box3& box, SDL_Color color) {
	double x = box.x;
	double y = box.y;
	double z = box.z;
	double w = box.w;
	double h = box.h;
	double d = box.d;

	// 8 вершин куба
	SDL_Point pts[8];
	pts[0] = { projectX(x, z), projectY(y, z) };             // передн€€ нижн€€ лева€
	pts[1] = { projectX(x + w, z), projectY(y, z) };          // передн€€ нижн€€ права€
	pts[2] = { projectX(x + w, z), projectY(y + h, z) };      // передн€€ верхн€€ права€
	pts[3] = { projectX(x, z), projectY(y + h, z) };          // передн€€ верхн€€ лева€
	pts[4] = { projectX(x, z + d), projectY(y, z + d) };      // задн€€ нижн€€ лева€
	pts[5] = { projectX(x + w, z + d), projectY(y, z + d) };  // задн€€ нижн€€ права€
	pts[6] = { projectX(x + w, z + d), projectY(y + h, z + d) }; // задн€€ верхн€€ права€
	pts[7] = { projectX(x, z + d), projectY(y + h, z + d) };     // задн€€ верхн€€ лева€

	SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a); // зелЄный

	// передн€€ грань
	SDL_RenderDrawLine(ren, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
	SDL_RenderDrawLine(ren, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
	SDL_RenderDrawLine(ren, pts[2].x, pts[2].y, pts[3].x, pts[3].y);
	SDL_RenderDrawLine(ren, pts[3].x, pts[3].y, pts[0].x, pts[0].y);

	// задн€€ грань
	SDL_RenderDrawLine(ren, pts[4].x, pts[4].y, pts[5].x, pts[5].y);
	SDL_RenderDrawLine(ren, pts[5].x, pts[5].y, pts[6].x, pts[6].y);
	SDL_RenderDrawLine(ren, pts[6].x, pts[6].y, pts[7].x, pts[7].y);
	SDL_RenderDrawLine(ren, pts[7].x, pts[7].y, pts[4].x, pts[4].y);

	// соедин€ем грани
	for (int i = 0; i < 4; i++) {
		SDL_RenderDrawLine(ren, pts[i].x, pts[i].y, pts[i + 4].x, pts[i + 4].y);
	}
}
void drawFilledFace(SDL_Renderer* ren, SDL_Point p0, SDL_Point p1, SDL_Point p2, SDL_Point p3, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	// «аливка как 2 треугольника: p0-p1-p2 и p2-p3-p0
	SDL_Vertex verts[6] = {
		{{(float)p0.x, (float)p0.y}, {r, g, b, a}, {0}},
		{{(float)p1.x, (float)p1.y}, {r, g, b, a}, {0}},
		{{(float)p2.x, (float)p2.y}, {r, g, b, a}, {0}},

		{{(float)p2.x, (float)p2.y}, {r, g, b, a}, {0}},
		{{(float)p3.x, (float)p3.y}, {r, g, b, a}, {0}},
		{{(float)p0.x, (float)p0.y}, {r, g, b, a}, {0}},
	};
	SDL_RenderGeometry(ren, nullptr, verts, 6, nullptr, 0);
}
void drawFilledBox(const Box3& box, SDL_Renderer* ren, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	// 1. передн€€ грань (XY at Z)
	SDL_Point f0 = { projectX(box.x, box.z), projectY(box.y, box.z) };
	SDL_Point f1 = { projectX(box.x + box.w, box.z), projectY(box.y, box.z) };
	SDL_Point f2 = { projectX(box.x + box.w, box.z), projectY(box.y + box.h, box.z) };
	SDL_Point f3 = { projectX(box.x, box.z), projectY(box.y + box.h, box.z) };
	drawFilledFace(ren, f0, f1, f2, f3, r, g, b, a);

	// 2. задн€€ грань (XY at Z + d)
	SDL_Point b0 = { projectX(box.x, box.z + box.d), projectY(box.y, box.z + box.d) };
	SDL_Point b1 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y, box.z + box.d) };
	SDL_Point b2 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	SDL_Point b3 = { projectX(box.x, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	drawFilledFace(ren, b0, b1, b2, b3, r * 0.6, g * 0.6, b * 0.6, a);

	// 3. лева€ грань (YZ at X)
	SDL_Point l0 = { projectX(box.x, box.z), projectY(box.y, box.z) };
	SDL_Point l1 = { projectX(box.x, box.z + box.d), projectY(box.y, box.z + box.d) };
	SDL_Point l2 = { projectX(box.x, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	SDL_Point l3 = { projectX(box.x, box.z), projectY(box.y + box.h, box.z) };
	drawFilledFace(ren, l0, l1, l2, l3, r * 0.7, g * 0.7, b * 0.7, a);

	// 4. права€ грань (YZ at X + w)
	SDL_Point r0 = { projectX(box.x + box.w, box.z), projectY(box.y, box.z) };
	SDL_Point r1 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y, box.z + box.d) };
	SDL_Point r2 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	SDL_Point r3 = { projectX(box.x + box.w, box.z), projectY(box.y + box.h, box.z) };
	drawFilledFace(ren, r0, r1, r2, r3, r * 0.7, g * 0.7, b * 0.7, a);

	// 5. верхн€€ грань (XZ at Y)
	SDL_Point t0 = { projectX(box.x, box.z), projectY(box.y, box.z) };
	SDL_Point t1 = { projectX(box.x + box.w, box.z), projectY(box.y, box.z) };
	SDL_Point t2 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y, box.z + box.d) };
	SDL_Point t3 = { projectX(box.x, box.z + box.d), projectY(box.y, box.z + box.d) };
	drawFilledFace(ren, t0, t1, t2, t3, r * 0.9, g * 0.9, b * 0.9, a);

	// 6. нижн€€ грань (XZ at Y + h)
	SDL_Point bt0 = { projectX(box.x, box.z), projectY(box.y + box.h, box.z) };
	SDL_Point bt1 = { projectX(box.x + box.w, box.z), projectY(box.y + box.h, box.z) };
	SDL_Point bt2 = { projectX(box.x + box.w, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	SDL_Point bt3 = { projectX(box.x, box.z + box.d), projectY(box.y + box.h, box.z + box.d) };
	drawFilledFace(ren, bt0, bt1, bt2, bt3, r * 0.5, g * 0.5, b * 0.5, a);
}