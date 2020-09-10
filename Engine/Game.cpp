#include "Game.h"

void Game::OnKeyPressed(KeyEvent &e)
{
	//printf("Key pressed from game");
}

void Game::OnKeyReleased(KeyEvent& e)
{
}

void Game::OnMouseMove(MouseMoveEvent& e)
{
	////printf("On Mouse move from game");
}

void Game::OnMouseClick(MouseClickEvent& e)
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

void Game::OnUpdate(UpdateEvent &e)
{
}

void Game::OnRender()
{
}

Game::Game(const std::string& name, int width, int height)
	:win(new Window(name, width, height, false)), width(width), height(height), vSync(false)
{
	
}
