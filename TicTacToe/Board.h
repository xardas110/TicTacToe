#pragma once
#include "../include/glm/glm.hpp"
#include "../Engine/Mesh.h"
#include <memory>
#include <xmmintrin.h>
#include <vector>

class Board
{
public:
	Board();
	enum Player
	{
		PlayerO,
		PlayerX,
		Open = -1,
		Size
	}currentPlayerTurn; //-> O always start
	enum GameState
	{
		WinnerO = Player::PlayerO,
		WinnerX = Player::PlayerX,
		NoWinner = -1
	};
private:
	const int gridLengthX;
	const int gridLengthY;
	const float gridSizeX;
	const float gridSizeY;
	int playerOScale;
	int playerXScale;
	glm::vec3 transation;
	std::unique_ptr<Mesh> grid;
	std::unique_ptr<Mesh> playerO;
	std::unique_ptr<Mesh> playerX; // will just use a quad for now for this, after logic is done I will make it a x or something cool
	__m128 tiles[3]; //Will use SIMD instructions for best performance

public:
	GameState		CheckForWinner();
	glm::vec3		GetTileCentrePosition(const int x, const int y);
	glm::vec3		GetTileCentrePosition(glm::vec3 xy);
	glm::vec3		TranslateToBoardSpace(glm::vec3 posWS);
	bool			SetTile(const int x, const int y, Player player);
	Player			IsTileTaken(const int x, const int y);
};

