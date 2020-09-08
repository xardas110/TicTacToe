#include "Game.h"

void Game::OnKeyPress()
{
	//printf("Key pressed from game");
}

void Game::OnMouseMove()
{
	////printf("On Mouse move from game");
}

void Game::OnMouseClick()
{
	//printf("Mouse clicked inside game");
}

int Game::Init()
{
	win->InitGLFW();
	win->game = this->shared_from_this();
	return 1;
}

int Game::OnLoad()
{
	return 1;
}

void Game::OnUpdate()
{
}

void Game::OnRender()
{
}

Game::Game(const std::string& name, int width, int height)
	:win(new Window(name, width, height, false)), width(width), height(height), vSync(false)
{
	
}
