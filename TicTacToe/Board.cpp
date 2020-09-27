#include "Board.h"
#include <iostream>
#include "../Engine/Debug.h"
#include <immintrin.h>
#include <inttypes.h>
#include "TileInfo.h"
#include <ctime>
Board::Board()
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), 
	boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)), 
	gridLengthX(3), gridLengthY(3), 
	gridSizeX(1.f), gridSizeY(1.f), 
	playerScaleX(0.8f), playerScaleY(0.8f), 
	Ai(std::shared_ptr<AIScripted>(new AIScripted((AIScripted::Player)Player::PlayerX, (AIScripted::Player)Player::PlayerO))), 
	currentPlayerTurn(Player::PlayerO)
{

	for (int i = 0; i < tiles.length(); i++)
		tiles[i] = glm::vec<3, int>(TileState::Open);
	
	BB.SetExtents(glm::vec3(gridLengthX * gridSizeX, gridLengthY * gridSizeY, 0.f));
	const auto gridXHalf = (gridLengthX * gridSizeX) * 0.5f;
	const auto gridYHalf = (gridLengthY * gridSizeY) * 0.5f;
	boardSpace = glm::translate(boardSpace, glm::vec3(gridXHalf, gridYHalf, 0.f));
	inverseBoard = glm::inverse(boardSpace);
}

Board::Board(std::shared_ptr<AIScripted> aiType)
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), 
	boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)), 
	gridLengthX(3), gridLengthY(3), 
	gridSizeX(1.f), gridSizeY(1.f), 
	playerScaleX(0.6f), playerScaleY(0.8f), 
	Ai(aiType), currentPlayerTurn(Player::PlayerO), tiles{0.f}
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
	if (gameState != GameState::OnGoing)
		return TileState::NotYourTurn;
	if (player != currentPlayerTurn)
		return TileState::NotYourTurn;

	auto localSpace = inverseWorld * glm::vec4(posWS, 1.f);
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
	gameState = CheckWinner(glm::vec<2, int>(floorX, floorY), player);
	
	return TileState::Success;
}

Board::TileState Board::SetTile(int x, int y, Player player)
{
	if (player != currentPlayerTurn)
		return TileState::NotYourTurn;

	if (gameState != GameState::OnGoing)
		return TileState::NotYourTurn;
	
	auto tState = IsTileTaken(x, y);
	if (tState != TileState::Open)
		return tState;

	std::cout << "Turn accepted " << y << " " << x <<  std::endl;
	tiles[y][x] = player;
	playerDrawList[player].push_back(GetTileCentrePositionWS(x, y));
	gameState = CheckWinner(glm::vec<2, int>(x, y), player);
	
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

Board::~Board()
{
	std::cout << "board destructor running" << std::endl;
	if (auto ai = Ai.lock())
	{ 
		ai->~AIScripted();
		Ai.~weak_ptr();
	}
	
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

void Board::Draw(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shaderTex, glm::mat4 projView)
{
	glm::mat4 boardMVP = projView * GetWorldSpace();
	shader->BindVec3("ambient", glm::vec3(0.2, 0.2f, 0.2f));
	shader->BindVec3("diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	shader->BindVec3("specular", glm::vec3(1.f, 1.f, 1.f));
	shader->BindFloat("shininess", 64.f);
	shader->BindMat4("worldspace", GetWorldSpace());
	shader->BindMat4("MVP", boardMVP);
	grid->Draw(0x0001);//draw lines
	
	shaderTex->Use();
	for (int j = 0; j < _countof(playerDrawList); j++)
	{ 
		playerTexture[j]->BindTexture();
		shaderTex->BindVec3("ambient", playerColor[j].ambient);
		shaderTex->BindVec3("specular", playerColor[j].specular);
		shaderTex->BindFloat("shininess", playerColor[j].shininess);
		for (int i = 0; i < playerDrawList[j].size(); i++)
		{
			auto model = glm::mat4(1.f);
			model = glm::translate(model, playerDrawList[j][i]);
			model = glm::scale(model, glm::vec3(playerScaleX, playerScaleY, 1.f));
			shaderTex->BindMat4("worldspace", model);
			shaderTex->BindMat4("MVP", projView * model);
			playerMesh[j]->Draw();
		}
	}
}

void Board::Init()
{
	grid = Mesh::CreateGrid(gridLengthX, gridLengthY, gridSizeX, gridSizeY);
	playerMesh[Player::PlayerO] = Mesh::CreateQuad();
	playerMesh[Player::PlayerX] = Mesh::CreateQuad();
	playerTexture[Player::PlayerO] = Texture::LoadTextureFromFile("../Textures/O.png");
	playerTexture[Player::PlayerX] = Texture::LoadTextureFromFile("../Textures/X.png");
	playerColor[Player::PlayerO] = Material::CreateMaterial(
		{ 0.0215f, 0.1745f, 0.0215f },
		{ 0.07568f, 0.61424f, 0.07568f },
		{ 1.f, 1.0f, 1.f },
		4096.0f
	);
	playerColor[Player::PlayerX] = Material::CreateMaterial(
		{ 0.1745f, 0.01175f, 0.01175f },
		{ 0.61424f, 0.04136f, 0.04136f },
		{ 1.0f, 1.0f, 1.0f },
		4096.0f
	);

	AISetTurn();
}

Board::GameState Board::CheckWinner(glm::vec<2, int> pos, Player player)
{
	using vec2 = glm::vec<2, int>;
	using vec3 = glm::vec<3, int>;
	//priority map for AI
	//row combinations
	std::vector<RowInfo> tInfo;//2 instructions
	//I can optimize this code better, but I was too lazy, I don't need to save this many positions and a vec space, just need 1 offset with the boards initial mem adress
	RowInfo r0, r1, r2, r3, r4;
	r0.r = vec3(tiles[pos.y][0], tiles[pos.y][1], tiles[pos.y][2]);
	r1.r = vec3(tiles[0][pos.x], tiles[1][pos.x], tiles[2][pos.x]);

	r0.tilePos[1] = vec2(pos.y, 0);
	r0.tilePos[0] = vec2(pos.y, 1);
	r0.tilePos[2] = vec2(pos.y, 2);
	r1.tilePos[1] = vec2(0, pos.x);
	r1.tilePos[0] = vec2(1, pos.x);
	r1.tilePos[2] = vec2(2, pos.x);
	tInfo.push_back(r0);
	tInfo.push_back(r1);

	const int sum = pos.y - pos.x;
	if (sum != 1 and sum != -1) //Need to do diagonal checks if this is true
	{
		if (pos.x == 1 && pos.y == 1) //4 combinations if its in the middle
		{
			r2.r = vec3(tiles[0][2], tiles[2][0], tiles[1][1]);
			r3.r = vec3(tiles[0][0], tiles[2][2], tiles[1][1]);
			r2.tilePos[2] = vec2(0, 2);
			r2.tilePos[1] = vec2(2, 0);
			r2.tilePos[0] = vec2(1, 1);
			r3.tilePos[2] = vec2(0, 0);
			r3.tilePos[1] = vec2(2, 2);
			r3.tilePos[0] = vec2(1, 1);
			tInfo.push_back(r2);
			tInfo.push_back(r3);
		}
		else{
			vec2 temp(2 - pos.x, 2 - pos.y);
			vec2 temp1 = (temp + pos) / 2; //change to multiplication cuz division require more cycles
			r4.r = vec3(tiles[temp.y][temp.x], tiles[temp1.y][temp1.x], tiles[pos.y][pos.x]);
			r4.tilePos[1] = vec2(temp.y, temp.x);
			r4.tilePos[0] = vec2(temp1.y, temp1.x);
			r4.tilePos[2] = vec2(pos.y, pos.x);
			tInfo.push_back(r4);
		}
	}
	
	for (int i = 0; i < tInfo.size(); i++) //max size for r is 4, but usually at 2-3
	{
		const auto rSum = tInfo[i].r.x + tInfo[i].r.y + tInfo[i].r.z;
		switch (rSum)
		{
			case 0:
			{
				std::cout << "Player o won" << std::endl;
				return GameState::Finished;
			}
			case 3:
			{
				std::cout << "Player x won" << std::endl;
				return GameState::Finished;
			}
			default:
			{
				tInfo[i].sum = rSum;
				break;
			}
		}
	}

	glm::vec<3, int> tilesSum = tiles[0] + tiles[1] + tiles[2];
	auto const totalSum = tilesSum.x + tilesSum.y  + tilesSum.z;
	if (totalSum == 4)
	{ 
		std::cout << "Game draw!" << std::endl;
		return GameState::Finished;
	}
	if (auto AI = Ai.lock())
		AI->CalculatePriorityMap(tInfo, tiles);
	SetNextPlayerTurn(player);
	return GameState::OnGoing;
}

void Board::AISetTurn()
{	
	if (auto AI = Ai.lock())
	{ 
		for (auto& pM : AI->priorityMap)
		{	
			for (int i = 0; i < _countof(pM.second.tilePos); i++)
			{
				auto const x = pM.second.tilePos[i].x;
				auto const y = pM.second.tilePos[i].y;
				if (SetTile(y, x, (Player)AI->aiSide) == TileState::Success)
					return;
			}
		}
		//this means the priority map has not given any value, so let the ai set a random pos
		std::srand((unsigned int)std::time(nullptr));
		int valX = rand() % 3;
		int valY = rand() % 3;
		SetTile(valX, valY, (Player)AI->aiSide);
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
