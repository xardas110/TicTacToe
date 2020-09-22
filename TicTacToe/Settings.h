#pragma once
#include "AIScripted.h"
#include "Board.h"
#include <memory>
struct Settings
{
public:
	enum GameTypes
	{
		PVP, PVE
	} gameType;
	enum Difficulty
	{
		Easy, Medium, Hard
	} difficulty = Difficulty::Hard;
	std::shared_ptr<AIScripted> Ai;
	Board::Player playerSide;
	Board::Player aiSide;
};