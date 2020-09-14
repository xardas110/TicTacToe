#pragma once
#include <../include/glm/glm.hpp>

struct tileInfo
{
public:
	int sum;
	glm::vec<3, int> r;
	glm::vec<2, int> tilePos[3];
};