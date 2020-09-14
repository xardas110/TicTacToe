#pragma once
//#include "../include/GLFW/glfw3.h"
#include "Game.h"
#include <vector>
#include <string>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
//Head of the program, this has control of everything(if I add everything)
class Application
{
private:
	void Init(std::shared_ptr<Game> g);
	std::shared_ptr<Window> CreateRenderWindow(const std::string name, int width, int height, bool vsync);
	int						SetGLFWCallback(std::shared_ptr<Window> w);
	static double			lastFrame;
	static double			totalTime;
	static uint64_t			frameNr;	
public:
	static void				Create(std::shared_ptr<Game> g);
	static					Application& Get();
	int						Run(std::shared_ptr<Game> g);	
	static void				Destroy();
	static unsigned int		LoadCubeMapTextures(std::vector<std::string> textures);
	std::shared_ptr<Window> callWindow;
};

