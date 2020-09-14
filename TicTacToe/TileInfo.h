#pragma once
#include <../include/glm/glm.hpp>

struct TileInfo
{
public:
	int sum;
	glm::vec<3, int> r;
	glm::vec<2, int> tilePos[3];
};