#include "AIScripted.h"
#include <iostream>
#include <inttypes.h>


AIScripted::AIScripted(Player playerSide, Player aiSide)
	:playerSide(playerSide), aiSide(aiSide)
{
}

void _vectorcall AIScripted::CalculatePriorityMap(std::vector<TileInfo> tInfo, const glm::mat<3, 3, int>& tiles)
{
	const int winningPos0 = aiSide + aiSide -3;
	const int defendingPos0 = playerSide + playerSide - 3;
	const int atkPos0 = aiSide - 6;
	const int defendingPos1 = playerSide - 6;
	const int uselessTile = playerSide + playerSide + aiSide;

	const int diagonalSum = tiles[0][0] + tiles[1][1] + tiles[2][2];
	const int r2Sum = tiles[2][0] + tiles[2][1] + tiles[2][2];

	if (diagonalSum == uselessTile && r2Sum == atkPos0) //special case
	{
		TileInfo tInfo;
		tInfo.tilePos[0] = glm::vec<2, int>(2, 0);
		tInfo.tilePos[1] = glm::vec<2, int>(2, 0);
		tInfo.tilePos[2] = glm::vec<2, int>(2, 0);
		priorityMap[2] = tInfo;
	}
	for (auto current : tInfo) //prio map is max4 in size, most cases its only3
	{
		if (current.sum == winningPos0)
			priorityMap[0] = current;
		else if (current.sum == defendingPos0)
			priorityMap[1] = current;
		else if (current.sum == defendingPos1)
			priorityMap[3] = current;
	}
}
