#pragma once
//#include "../include/GLFW/glfw3.h"
#include "Game.h"

//Head of the program, this has control of everything(if I add everything)
class Application
{
private:
	void Init(std::shared_ptr<Game> g);
	std::shared_ptr<Window> CreateRenderWindow(const std::string name, int width, int height, bool vsync);
	int						SetGLFWCallback(std::shared_ptr<Window> w);
	
public:
	static void				Create(std::shared_ptr<Game> g);
	static					Application& Get();
	int						Run(std::shared_ptr<Game> g);	
	static void				Destroy();
	std::shared_ptr<Window> callWindow;
};

