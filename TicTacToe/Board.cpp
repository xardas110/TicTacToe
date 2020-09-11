#include "Board.h"
#include <iostream>
#include "../Engine/Debug.h"
#include <immintrin.h>
Board::Board()
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)), gridLengthX(3), gridLengthY(3), gridSizeX(1.f), gridSizeY(1.f), tiles{-1}, playerScaleX(0.6f), playerScaleY(0.6f)
{
	//Only 64 bit support
	boardCombinations.push_back(_mm256_setr_epi64x((__int64)&tiles[0].m128i_i32[0], (__int64)&tiles[1].m128i_i32[1], (__int64)&tiles[2].m128i_i32[2], (__int64)&tiles[3].m128i_i32[3]));
	boardCombinations.push_back(_mm256_setr_epi64x((__int64)&tiles[0].m128i_i32[3], (__int64)&tiles[1].m128i_i32[2], (__int64)&tiles[1].m128i_i32[1], (__int64)&tiles[3].m128i_i32[0]));
	for (int i = 0; i < _countof(tiles); i++)
	{ 
		tiles[i] = _mm_set1_epi32(TileState::Open);
		boardCombinations.push_back(_mm256_setr_epi64x((__int64)&tiles[i].m128i_i32[0], (__int64)&tiles[i].m128i_i32[1], (__int64)&tiles[i].m128i_i32[2], (__int64)&tiles[i].m128i_i32[3]));
		boardCombinations.push_back(_mm256_setr_epi64x((__int64)&tiles[0].m128i_i32[i], (__int64)&tiles[1].m128i_i32[i], (__int64)&tiles[2].m128i_i32[i], (__int64)&tiles[3].m128i_i32[i]));
	}
	BB.SetExtents(glm::vec3(gridLengthX, gridLengthY, 0.f));
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
	tiles[floorY].m128i_i32[floorX] = player;
	playerDrawList[player].push_back(GetTileCentrePositionWS(floorX, floorY));
	//check winner, if no winner set player turn, dont set player turn before checking for a winner, cuz of optimization reasons
	winnerState = CheckWinner(player);
	SetPlayerTurn(player);

	return TileState::Success;
}

Board::TileState Board::IsTileTaken(const int x, const int y)
{
	//if (x >= gridLengthX || y >= gridLengthY) //this code should never be necessary since I use ray intersection with the board as a bounding box
	//	return TileState::OutSideBounds;
	if (tiles[y].m128i_i32[x] != Open)
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

Board::WinnerState Board::CheckWinner(Player player)
{
	//time for some simd functions
	__m128i mask1 = _mm_set1_epi32(player);
	for (int i = 0; i < boardCombinations.size(); i++)
	{		
		auto currentTile = _mm_setr_epi32(*(int*)boardCombinations[i].m256i_i64[0], *(int*)boardCombinations[i].m256i_i64[1], *(int*)boardCombinations[i].m256i_i64[2], *(int*)boardCombinations[i].m256i_i64[3]);
		const auto cmp = _mm_cmpeq_epi32(mask1, currentTile);
		const auto mask = _mm_movemask_epi8(cmp);
		if (((mask & 0xFFF) == 0xFFF))
		{ 
			std::cout << "Player " << player << " won" << std::endl;
			return (WinnerState)player;
		}

	}
	return WinnerState::NoWinner;
}

void Board::SetPlayerTurn(Player player)
{
	if (player == Player::PlayerO)
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
