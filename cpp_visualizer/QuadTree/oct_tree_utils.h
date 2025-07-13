#pragma once
#include "oct_tree.h"
#include "helper.h"
#include <vector>
#include <chrono>

void CreateRandomPointOct(OctTree& tree);
void TestOct(const OctTree& tree);

class VisualOct {
private:
	int idx_color;
	std::vector<SDL_Color> palette;
	std::vector<std::pair<Box3, SDL_Color>> visualNodes;
public:
	VisualOct();

	void addBoxes(const std::vector<Box3>& boxes);
	void addBox(const Box3& b);
	void removeBox(const Box3& b);

	void draw();
};