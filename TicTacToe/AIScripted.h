#pragma once
#include "../include/glm/glm.hpp"
#include <map>
#include <vector>
#include "TileInfo.h"
class AIScripted
{
	friend class Board;
private:
	enum Player
	{
		PlayerO,
		PlayerX,
		Size
	}playerSide = Player::PlayerO, aiSide = Player::PlayerX;
	std::map<int, TileInfo> priorityMap; //Will hold move priorities for 2 turns, 1 for playerO and 1 for playerX
public:
	AIScripted(Player playerSide, Player aiSide);
	void _vectorcall CalculatePriorityMap(std::vector<TileInfo> tinfo, const glm::mat<3, 3, int>& tiles);

};

