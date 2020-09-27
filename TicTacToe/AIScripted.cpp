#include "AIScripted.h"
#include <iostream>
#include <inttypes.h>
#include <ctime>

AIScripted::AIScripted(Player playerSide, Player aiSide)
	:playerSide(playerSide), aiSide(aiSide)
{
}

void _vectorcall AIScripted::CalculatePriorityMap(std::vector<RowInfo> tInfo, const glm::mat<3, 3, int>& tiles)
{
	std::cout << "calcprio map" << std::endl;
	const int winningPos0 = aiSide + aiSide - 3;
	const int defendingPos0 = playerSide + playerSide - 3;
	const int atkPos0 = aiSide - 6;
	const int defendingPos1 = playerSide - 6;
	const int drawTile = playerSide + aiSide - 3;
	const int uselessTile = playerSide + playerSide + aiSide;
	
	const int diagonalSum = tiles[0][0] + tiles[1][1] + tiles[2][2];
	const int r2Sum = tiles[2][0] + tiles[2][1] + tiles[2][2];

	if (diagonalSum == uselessTile && r2Sum == atkPos0) //special case
	{
		RowInfo tInfo;
		tInfo.tilePos[0] = glm::vec<2, int>(2, 0);
		tInfo.tilePos[1] = glm::vec<2, int>(2, 0);
		tInfo.tilePos[2] = glm::vec<2, int>(2, 0);
		priorityMap[2] = tInfo;
	}
	for (auto current : tInfo) //prio map is max5 in size, most cases its only3
	{
		if (current.sum == winningPos0)
			priorityMap[0] = current;
		else if (current.sum == defendingPos0)
			priorityMap[1] = current;
		else if (current.sum == defendingPos1)
			priorityMap[3] = current;
		else if (current.sum == drawTile)
			priorityMap[4] = current;
	}
}

AIScripted::~AIScripted()
{
	std::cout << "destruct running" << std::endl;
	priorityMap.clear();
}

AIEasy::AIEasy(Player playerSide, Player aiSide)
	:AIScripted(playerSide, aiSide)
{
}

void _vectorcall AIEasy::CalculatePriorityMap(std::vector<RowInfo> tinfo, const glm::mat<3, 3, int>& tiles)
{
	const int winningPos0 = aiSide + aiSide - 3;
	const int defendingPos0 = playerSide + playerSide - 3;
	const int atkPos0 = aiSide - 6;
	const int defendingPos1 = playerSide - 6;
	const int drawTile = playerSide + aiSide - 3;

	for (auto current : tinfo) //prio map is max5 in size, most cases its only3
	{
		if (current.sum == winningPos0)
			priorityMap[0] = current;
		else if (current.sum == defendingPos0)
			priorityMap[1] = current;
		else if (current.sum == defendingPos1)
			priorityMap[5] = current;
		else if (current.sum == atkPos0)
			priorityMap[6] = current;
		else if (current.sum == drawTile)
			priorityMap[7] = current;

	}
	
	RowInfo rInfo[3];
	std::srand((unsigned int)std::time(nullptr));
	for (int i= 0, k = 2; i< _countof(rInfo); i++, k++)
	{ 
		for (int j = 0; j < _countof(rInfo); j++)
		{		
			int valX = rand() % 3;
			int valY = rand() % 3;
			rInfo->tilePos[j] = glm::vec<2, int>(valX, valY);
			std::cout << "rand vals: " << valX << " "  << valY << std::endl;
			
		}
		rInfo[i].sum = 100;
		priorityMap[k] = rInfo[i];
	}
}

AINormal::AINormal(Player playerSide, Player aiSide)
	:AIScripted(playerSide, aiSide)
{
}

void _vectorcall AINormal::CalculatePriorityMap(std::vector<RowInfo> tinfo, const glm::mat<3, 3, int>& tiles)
{
	const int winningPos0 = aiSide + aiSide - 3;
	const int defendingPos0 = playerSide + playerSide - 3;
	const int atkPos0 = aiSide - 6;
	const int defendingPos1 = playerSide - 6;
	const int drawTile = playerSide + aiSide - 3;

	for (auto current : tinfo) //prio map is max5 in size, most cases its only3
	{
		if (current.sum == winningPos0)
			priorityMap[0] = current;
		else if (current.sum == defendingPos0)
			priorityMap[1] = current;
		else if (current.sum == defendingPos1)
			priorityMap[6] = current;
		else if (current.sum == atkPos0)
			priorityMap[5] = current;
		else if (current.sum == drawTile)
			priorityMap[7] = current;

	}
	std::srand((unsigned int)std::time(nullptr));

	rInfo[0].tilePos[0] = glm::vec2(1, 1);
	rInfo[0].tilePos[1] = glm::vec2(0, 0);
	rInfo[0].tilePos[2] = glm::vec2(2, 2);
	rInfo[1].tilePos[0] = glm::vec2(2, 0);
	rInfo[1].tilePos[1] = glm::vec2(0, 2);
	rInfo[1].tilePos[2] = glm::vec2(1, 1);
	int randVal = std::rand() % 2;
	priorityMap[2] = rInfo[randVal];
	randVal = std::rand() % 2;
	priorityMap[3] = rInfo[randVal];

	for (int i = 0, k = 2; i < _countof(rInfo); i++, k++)
	{
		for (int j = 0; j < _countof(rInfo); j++)
		{
			int valX = rand() % 3;
			int valY = rand() % 3;
			rInfo->tilePos[j] = glm::vec<2, int>(valX, valY);
			std::cout << "rand vals: " << valX << " " << valY << std::endl;

		}
		priorityMap[k] = rInfo[i];
	}
}
