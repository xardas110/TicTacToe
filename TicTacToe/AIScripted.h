#pragma once
#include "../include/glm/glm.hpp"
#include <map>
#include <vector>
#include "TileInfo.h"
class AIScripted
{
	friend class Board;
public:
	enum Player
	{
		PlayerO,
		PlayerX,
		Size
	}playerSide = Player::PlayerO, aiSide = Player::PlayerX;
public:
	AIScripted(Player playerSide, Player aiSide);
	virtual void _vectorcall CalculatePriorityMap(std::vector<RowInfo> tinfo, const glm::mat<3, 3, int>& tiles);
	~AIScripted();
protected:
	std::map<int, RowInfo> priorityMap; //Will hold move priorities for 2 turns, 1 for playerO and 1 for playerX

};

class AIEasy : public AIScripted
{
private: RowInfo rInfo[3];
public:
	AIEasy(Player playerSide, Player aiSide);
	virtual void _vectorcall CalculatePriorityMap(std::vector<RowInfo> tinfo, const glm::mat<3, 3, int>& tiles) override;
};

class AINormal : public AIScripted
{
private: RowInfo rInfo[3];
public:
	AINormal(Player playerSide, Player aiSide);
	virtual void _vectorcall CalculatePriorityMap(std::vector<RowInfo> tinfo, const glm::mat<3, 3, int>& tiles) override;
};