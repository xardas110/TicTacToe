#include "Board.h"
#include <iostream>
#include "../Engine/Debug.h"
#include <immintrin.h>

Board::Board()
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)), gridLengthX(3), gridLengthY(3), gridSizeX(1.f), gridSizeY(1.f), playerScaleX(0.6f), playerScaleY(0.6f)
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

	std::cout << "Turn accepted" << std::endl;
	tiles[y][x] = player;
	playerDrawList[player].push_back(GetTileCentrePositionWS(x, y));
	winnerState = CheckWinner(glm::vec<2, int>(x, y), player);
	

	return TileState::Success;
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
	//Looks like this isn't optimized cuz of lots of code, but it only uses a few instructions for everything
	using vec2 = glm::vec<2, int>;
	using vec4 = glm::vec<4, __int64>; //w will be used as open tiles for the ai, byte 1 = tile0.x, byte2 = tile0.y, byte 3 = tile1.x, byte 4 = tile1.y
	using mat3 = glm::mat<3, 3, int>;

	//priority map for AI
	std::map<int, vec4> priorityMap;
	std::vector<vec4> r;

	char bytesh[]{ (char)pos.y, (char)0, (char)pos.y, (char)1, (char)pos.y, (char)2 };
	char bytesv[]{ (char)0, (char)pos.x, (char)1, (char)pos.x, (char)2, (char)pos.x };
	char bytes[6]{};
	char bytes1[6]{};
	char bytes2[6]{};
	r.push_back(vec4(tiles[pos.y][0], tiles[pos.y][1], tiles[pos.y][2], (__int64)bytesh));
	r.push_back(vec4(tiles[0][pos.x], tiles[1][pos.x], tiles[2][pos.x], (__int64)bytesv));
	std::cout << "pos.x value: " << pos.x << " pos.y value" << pos.y << std::endl;
	const int sum = pos.y - pos.x;
	
	if (sum != 1 and sum != -1)
	{
		if (pos.x == 1 && pos.y == 1) //4 combinations if its in the middle
		{
			char buff1[]{ (char)0, (char)2 , (char)2 , (char)0, (char)1 , (char)1 };
			char buff2[]{ (char)0, (char)0 , (char)2 , (char)2, (char)1 , (char)1 };
			memcpy(bytes1, buff1, 6);
			memcpy(bytes2, buff2, 6);
			r.push_back(vec4(tiles[bytes1[0]][bytes1[1]], tiles[bytes1[2]][bytes1[3]],tiles[1][1], (__int64)bytes1));
			r.push_back(vec4(tiles[bytes2[0]][bytes2[1]], tiles[bytes2[2]][bytes2[3]], tiles[1][1], (__int64)bytes2));
		}
		else{
			vec2 temp(2 - pos.x, 2 - pos.y);
			vec2 temp1 = (temp + pos) / 2; //change to multiplication cuz division requires more cycles
			char buff3[] = { (char)temp.y, (char)temp.x , (char)temp1.y , (char)temp1.x, (char)pos.y, (char)pos.x };
			memcpy(bytes, buff3, 6);
			r.push_back(vec4(tiles[bytes[0]][bytes[1]], tiles[bytes[2]][bytes[3]], tiles[bytes[4]][bytes[5]], (__int64)bytes));
		}
	}
	
	for (int i = 0; i < r.size(); i++)
	{
		const auto rSum = r[i].x + r[i].y + r[i].z;
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
		case -1: //11-3=-1
		{
			priorityMap[0] = r[i];
			std::cout << "//11-3 =-1, player x is about to win" << std::endl;
		}
		break;
		case -3: //00-3=-3
		{
			priorityMap[1] = r[i];
			std::cout << "//00-3=-3 player o is about to win" << std::endl;
		}
		break;
		case -6:
		{
			priorityMap[2] = r[i];
		}
		break;
		case -5:
		{
			priorityMap[2] = r[i];
		}
		break;
		case -2:
		{
			priorityMap[4] = r[i];
		}
		break;
		default:
			break;
		}
	}

	SetNextPlayerTurn(player);
	for (auto &pM : priorityMap)
	{ 
		auto a = pM.second.w;
		char* p = (char*)a;
		TileState sate;
		//const auto sum = tiles[p[0]][p[1]] + tiles[p[2]][p[3]] + tiles[p[4]][p[5]];
		sate = SetTile(p[3], p[2], Player::PlayerX);
		std::cout << "state 1: " << sate << std::endl;
		if (sate == TileState::Taken)
		{
			sate = SetTile(p[1], p[0], Player::PlayerX);
			std::cout << "state 2: " << sate << std::endl;
			if (sate == TileState::Taken)
			{
				sate = SetTile(p[5], p[4], Player::PlayerX);
				std::cout << "state 3: " << sate << std::endl;
			}
		}
		std::cout << sate << std::endl;
	}
	
	return WinnerState::NoWinner;

}

void Board::AICalculateTurn(std::map<int, glm::vec<3, int>> priorityMap)
{
	


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
