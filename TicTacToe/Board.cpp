#include "Board.h"
#include <iostream>
#include "../Engine/Debug.h"
#include <immintrin.h>
#include <inttypes.h>
#include "TileInfo.h"
Board::Board()
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)), gridLengthX(3), gridLengthY(3), gridSizeX(1.f), gridSizeY(1.f), playerScaleX(0.6f), playerScaleY(0.6f), Ai((AIScripted::Player)Player::PlayerO, (AIScripted::Player)Player::PlayerX)
{

	for (int i = 0; i < tiles.length(); i++)
		tiles[i] = glm::vec<3, int>(TileState::Open);
	
	BB.SetExtents(glm::vec3(gridLengthX * gridSizeX, gridLengthY * gridSizeY, 0.f));
	const auto gridXHalf = (gridLengthX * gridSizeX) * 0.5f;
	const auto gridYHalf = (gridLengthY * gridSizeY) * 0.5f;
	boardSpace = glm::translate(boardSpace, glm::vec3(gridXHalf, gridYHalf, 0.f));
	inverseBoard = glm::inverse(boardSpace);
}

Board::TileState Board::SetTile(glm::vec3 posWS, Player player)
{
	if (winnerState != WinnerState::NoWinner)
		return TileState::NotYourTurn;
	if (player != currentPlayerTurn)
		return TileState::NotYourTurn;

	auto localSpace = inverseWorld * glm::vec4(posWS, 1.f);
	localSpace.w = 1.f;
	auto boardSpace = this->boardSpace * glm::vec4(localSpace);
	boardSpace.y = (gridLengthY * gridSizeY) - boardSpace.y;//Gotta invert y so I can use it inside an array, where top of the board has y as 0 and not gridylength-1		
	//std::cout << localSpace.x << " " << localSpace.y << " " << localSpace.z << std::endl;
	//std::cout << boardSpace.x << " " << boardSpace.y << " " << boardSpace.z << std::endl;

	const int floorX = static_cast<int>(floorf(boardSpace.x));//this is safe, cause my raycast has a intersection point that can't be the same or larger than the board bounds
	const int floorY = static_cast<int>(floorf(boardSpace.y));//another option would be to add 9 bounding boxes for each tile, but that sure wont be as optimized as this
	//std::cout << floorX << " " << floorY << std::endl;

	if (IsTileTaken(floorX, floorY) != TileState::Open)
		return TileState::Taken;

	std::cout << "Turn accepted" << std::endl;
	tiles[floorY][floorX] = player;
	playerDrawList[player].push_back(GetTileCentrePositionWS(floorX, floorY));
	winnerState = CheckWinner(glm::vec<2, int>(floorX, floorY), player);
	
	return TileState::Success;
}

Board::TileState Board::SetTile(int x, int y, Player player)
{
	if (winnerState != WinnerState::NoWinner)
		return TileState::NotYourTurn;
	if (player != currentPlayerTurn)
		return TileState::NotYourTurn;

	if (IsTileTaken(x, y) == TileState::Taken)
		return TileState::Taken;
	if (IsTileTaken(x, y) == TileState::OutSideBounds)
	{
		return TileState::OutSideBounds;
	}

	std::cout << "Turn accepted " << y << " " << x <<  std::endl;
	tiles[y][x] = player;
	playerDrawList[player].push_back(GetTileCentrePositionWS(x, y));
	winnerState = CheckWinner(glm::vec<2, int>(x, y), player);
	
	return TileState::Success;
}

void Board::SetTurn(glm::vec3 posWS, Player player)
{
	TileState state;
	state = SetTile(posWS, player);
	if (state == TileState::Success)
	{
		AISetTurn();
	}
}

Board::TileState Board::IsTileTaken(const int x, const int y)
{
	if (x >= gridLengthX || y >= gridLengthY) //this code should never be necessary since I use ray intersection with the board as a bounding box
		return TileState::OutSideBounds;
	if (tiles[y][x] != Open)
		return TileState::Taken;
	return TileState::Open;
}

void Board::SetTranslate(glm::vec3 translate)
{
	worldSpace = glm::translate(worldSpace, translate);
	inverseWorld = glm::inverse(worldSpace);
	BB.SetCenter(worldSpace[3]);
	//PrintVector(BB.Extents);
}

void Board::SetScale(glm::vec3 scale)
{
	worldSpace = glm::scale(worldSpace, scale);
	inverseWorld = glm::inverse(worldSpace);
	const auto totalScale = glm::vec3(worldSpace[0][0], worldSpace[1][1], worldSpace[2][2]);
	BB.SetExtents(totalScale * glm::vec3(gridLengthX, gridLengthY, 0.f)); // haven't added a better function inside my bounding class, will do this later if I get time
	playerScaleX *= totalScale.x;
	playerScaleY *= totalScale.y;
}

const Bounding::Box Board::GetBoundingBox() const
{
	return BB;
}

const glm::mat4& Board::GetWorldSpace()
{
	return worldSpace;
}

void Board::Draw(std::shared_ptr<Shader> shader, glm::mat4 projView)
{
	shader->BindVec3("color", glm::vec3(1.f, 1.f, 1.f));
	grid->Draw(0x0001);//draw lines
	
	for (int j = 0; j < _countof(playerDrawList); j++)
	{ 
		shader->BindVec3("color", playerColor[j]);
		for (int i = 0; i < playerDrawList[j].size(); i++)
		{
			auto model = glm::mat4(1.f);
			model = glm::translate(model, playerDrawList[j][i]);
			model = glm::scale(model, glm::vec3(playerScaleX, playerScaleY, 1.f));
			shader->BindMat4("MVP", projView * model);
			playerMesh[j]->Draw();
		}
	}

}

void Board::Init()
{
	grid = Mesh::CreateGrid(gridLengthX, gridLengthY, gridSizeX, gridSizeY);
	playerMesh[Player::PlayerO] = Mesh::CreateCircle(16);//16 circle vertex edges
	playerMesh[Player::PlayerX] = Mesh::CreateQuad();
	playerColor[Player::PlayerO] = glm::vec3(0.f, 1.f, 0.f);
	playerColor[Player::PlayerX] = glm::vec3(1.f, 0.f, 0.f);
}

Board::WinnerState Board::CheckWinner(glm::vec<2, int> pos, Player player)
{
	using vec2 = glm::vec<2, int>;
	using vec3 = glm::vec<3, int>;

	//priority map for AI
	//row combinations
	std::vector<tileInfo> tInfo;//2 instructions
	//inserting tile x,y locations inside a bytebuffer
	tileInfo r0, r1, r2, r3, r4;
	r0.r = vec3(tiles[pos.y][0], tiles[pos.y][1], tiles[pos.y][2]);
	r1.r = vec3(tiles[0][pos.x], tiles[1][pos.x], tiles[2][pos.x]);

	r0.tilePos[0] = vec2(pos.y, 0);
	r0.tilePos[1] = vec2(pos.y, 1);
	r0.tilePos[2] = vec2(pos.y, 2);
	r1.tilePos[0] = vec2(0, pos.x);
	r1.tilePos[1] = vec2(1, pos.x);
	r1.tilePos[2] = vec2(2, pos.x);
	tInfo.push_back(r0);
	tInfo.push_back(r1);
	const int sum = pos.y - pos.x;
	
	if (sum != 1 and sum != -1)
	{
		if (pos.x == 1 && pos.y == 1) //4 combinations if its in the middle
		{
			r2.r = vec3(tiles[0][2], tiles[2][0], tiles[1][1]);
			r3.r = vec3(tiles[0][0], tiles[2][2], tiles[1][1]);
			r2.tilePos[0] = vec2(0, 2);
			r2.tilePos[1] = vec2(2, 0);
			r2.tilePos[2] = vec2(1, 1);
			r3.tilePos[0] = vec2(0, 0);
			r3.tilePos[1] = vec2(2, 2);
			r3.tilePos[2] = vec2(1, 1);
			tInfo.push_back(r2);
			tInfo.push_back(r3);
		}
		else{
			vec2 temp(2 - pos.x, 2 - pos.y);
			vec2 temp1 = (temp + pos) / 2; //change to multiplication cuz division requires more cycles
			r4.r = vec3(tiles[temp.y][temp.x], tiles[temp1.y][temp1.x], tiles[pos.y][pos.x]);
			r4.tilePos[0] = vec2(temp.y, temp.x);
			r4.tilePos[1] = vec2(temp1.y, temp1.x);
			r4.tilePos[2] = vec2(pos.y, pos.x);
			tInfo.push_back(r4);
		}
	}
	
	for (int i = 0; i < tInfo.size(); i++) //max size for r is 4, but usually at 2-3
	{
		const auto rSum = tInfo[i].r.x + tInfo[i].r.y + tInfo[i].r.z;
		std::cout << "row sum: " << rSum << std::endl;
		switch (rSum)
		{
			case 0:
				std::cout << "Player o won" << std::endl;
				return WinnerState::O;
				break;
			case 3:
				std::cout << "Player x won" << std::endl;
				return WinnerState::X;
				break;
			default:
			{
				tInfo[i].sum = rSum;
				break;
			}
		}
	}
	Ai.CalculatePriorityMap(tInfo);
	SetNextPlayerTurn(player);
	return WinnerState::NoWinner;

}

void Board::AISetTurn()
{
	for (auto& pM : Ai.priorityMap)
	{
		std::cout << "Priority map priorities: " << pM.first << std::endl;
		TileState sate;
		auto const x0 = pM.second.tilePos[1].x;
		auto const y0 = pM.second.tilePos[1].y;
		sate = SetTile(y0, x0, (Player)Ai.aiSide);
		std::cout << "state 1: " << sate << std::endl;
		if (sate != TileState::Success)
		{
			auto const x1 = pM.second.tilePos[0].x;
			auto const y1 = pM.second.tilePos[0].y;
			sate = SetTile(y1, x1, (Player)Ai.aiSide);
			std::cout << "state 2: " << sate << std::endl;
			if (sate != TileState::Success)
			{
				auto const x2 = pM.second.tilePos[2].x;
				auto const y2 = pM.second.tilePos[2].y;
				sate = SetTile(y2, x2, (Player)Ai.aiSide);
				std::cout << "state 3: " << sate << std::endl;
			}
			else
				return;
		}
		else
			return;
		std::cout << sate << std::endl;
	}

}

void Board::SetNextPlayerTurn(Player currentPlayer)
{
	if (currentPlayer == Player::PlayerO)
		currentPlayerTurn = Player::PlayerX;
	else
		currentPlayerTurn = Player::PlayerO;
}

glm::vec3 Board::GetTileCentrePositionWS(const int x, const int y)
{	
	auto const xCentre = (float)x + gridSizeX * 0.5f;
	const auto yCentre = (float)y + gridSizeY * 0.5f;
	auto tileCentreWS = glm::vec4(xCentre, (((float)gridLengthY*gridSizeY)) - yCentre, 0.f, 1.f);
	tileCentreWS = inverseBoard * tileCentreWS;
	tileCentreWS = worldSpace * tileCentreWS;
	return tileCentreWS;
}
