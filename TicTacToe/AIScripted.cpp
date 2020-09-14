#include "AIScripted.h"
#include <iostream>
#include <inttypes.h>


AIScripted::AIScripted(Player playerSide, Player aiSide)
	:playerSide(playerSide), aiSide(aiSide)
{
}

void AIScripted::CalculatePriorityMap(std::vector<tileInfo> tInfo)
{
	const int winningPos = aiSide + aiSide -3;
	const int defendingPos0 = playerSide + playerSide - 3;
	const int defendingPos1 = playerSide - 6;
	for (auto current : tInfo) 
	{
		if (current.sum == winningPos)
			priorityMap[0] = current;
		else if (current.sum == defendingPos0)
			priorityMap[15] = current;
		else if (current.sum == defendingPos1)
			priorityMap[20] = current;
	}
}
