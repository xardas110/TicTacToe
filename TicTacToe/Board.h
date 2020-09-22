#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../Engine/Mesh.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Shader.h"
#include "../Engine/Material.h"
#include "../Engine/Texture.h"
#include <memory>
#include <xmmintrin.h>
#include <vector>
#include <map>
#include "AIScripted.h"


class Board
{
public:
	enum Player
	{
		PlayerO,
		PlayerX,
		Size
	};
	enum GameState
	{
		OnGoing,
		Finished 
	};
	enum TileState
	{
		Taken,
		Open = -3,
		NotYourTurn,
		OutSideBounds,
		Success = 10
	};
	Board();
	~Board();
	Board(std::shared_ptr<AIScripted> aiType);//nullptr for aitype for no AI
	void SetTurn(glm::vec3 posWS, Player player);
	void SetTranslate(glm::vec3 translate);
	void SetScale(glm::vec3 scale);
	const Bounding::Box GetBoundingBox() const;
	const glm::mat4 &GetWorldSpace();
	
	void Draw(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shaderTex, glm::mat4 projview);
	void Init();
private:
	std::weak_ptr<AIScripted> Ai;
	TileState SetTile(glm::vec3 posWS, Player player);
	TileState SetTile(int x, int y, Player player);
	Player currentPlayerTurn;
	GameState gameState = GameState::OnGoing;
	GameState _vectorcall CheckWinner(glm::vec<2, int> pos, Player player);
	void AISetTurn();
	void SetNextPlayerTurn(Player currentPlayer);
	const int gridLengthX; //------------------------------------------------------------------------------------------------------------------------------------------------------//
	const int gridLengthY; //All these positions are done inside board space,and they will be constants for tictactoe, 3*3 tiles and gridsize is 1, for fastest possible array access
	const float gridSizeX;
	const float gridSizeY; //------------------------------------------------------------------------------------------------------------------------------------------------------//
	float playerScaleX, playerScaleY;
	std::vector<glm::vec3> playerDrawList[Player::Size];
	std::unique_ptr<Mesh> playerMesh[Player::Size];
	std::shared_ptr<Texture> playerTexture[Player::Size];
	std::unique_ptr<Mesh> grid;
	Material playerColor[Player::Size];
	
	glm::mat<3,3, int> tiles;
	glm::vec3 GetTileCentrePositionWS(const int x, const int y);
	Bounding::Box BB; //Bounding box that will be the board;
//inverse world gives me localspace
	glm::mat4 worldSpace, inverseWorld; //keeping everything in 3d calculations just in case I want to make this 3d instead, with intel SIMD instructions will basically be as good optimized as single instructions anyways -> TODO for a later date
	glm::mat4 boardSpace, inverseBoard; //for optimized grid calculations, top left:(0, 0)	bot right:(gridLengthX, gridLengthY) for fast array access
	TileState IsTileTaken(const int x, const int y);
};

