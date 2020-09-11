#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../Engine/Mesh.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Shader.h"
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
		Size
	}; //-> O always start
	enum GameState
	{
		OnGoing,
		Finished //by default the winner will be the guy who did thelast turn
	};
	enum TileState
	{
		Taken,
		Open = -3,
		NotYourTurn,
		OutSideBounds,
		Success
	};
	enum WinnerState
	{
		O, X,
		NoWinner
	};


	TileState SetTile(glm::vec3 posWS, Player player);	
	void SetTranslate(glm::vec3 translate);
	void SetScale(glm::vec3 scale);
	const Bounding::Box GetBoundingBox() const;
	const glm::mat4 &GetWorldSpace();
	
	void Draw(std::shared_ptr<Shader> shader, glm::mat4 projview);
	void Init();
	
private:
	Player currentPlayerTurn = Player::PlayerO;
	GameState gameState = GameState::OnGoing;
	WinnerState winnerState = WinnerState::NoWinner;
	WinnerState _vectorcall CheckWinner(Player player);
	void SetPlayerTurn(Player player);
	const int gridLengthX;
	const int gridLengthY; //All these positions are done inside board space,and they will be constants for tictactoe, 3*3 tiles and gridsize is 1, for fastest possible array access
	float playerScaleX, playerScaleY;
	const float gridSizeX;
	const float gridSizeY;
	std::vector<glm::vec3> playerDrawList[2];
	std::unique_ptr<Mesh> playerMesh[Player::Size];
	glm::vec3 playerColor[Player::Size];
	std::unique_ptr<Mesh> grid;
	 __m128i tiles[4];// would be simpler to have a matrix class for this datatype
	 std::vector<__m256i> boardCombinations;
	glm::vec3 GetTileCentrePositionWS(const int x, const int y);
	Bounding::Box BB; //Bounding box that will be the board;
//inverse world gives me localspace
	glm::mat4 worldSpace, inverseWorld; //keeping everything in 3d calculations just in case I want to make this 3d instead, with intel SIMD instructions will basically be as good optimized as single instructions anyways -> TODO for a later date
	glm::mat4 boardSpace, inverseBoard; //for optimized grid calculations, top left 0, 0 - bot right gridX-1.f, gridY-1.f for fast array access
	TileState IsTileTaken(const int x, const int y);

};

