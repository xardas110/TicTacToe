#pragma once
#include "Window.h"
#include <memory>
#include <string>
//I wont inherit window here cause I might want more than 1 window later
class Game : public std::enable_shared_from_this<Game>
{
private:
	std::string name;
	int width, height, vSync;
	
public:
	std::shared_ptr<Window> win;
	virtual int  Init();
	Game(const std::string& name, int width, int height);
	virtual int OnLoad();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnKeyPress();
	virtual void OnMouseMove();
	virtual void OnMouseClick();
protected:
	
};

