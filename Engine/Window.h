#pragma once
#include <Windows.h>
#include <string>
#include <GLFW/glfw3.h>
#include <memory>
class Game;
class Window : public std::enable_shared_from_this<Window>
{
private:
	HWND				hwnd;
	GLFWwindow*			window;//using a wrapper lib for ogl/windows connection for now because its cumbersome work to link them
	std::string			name;
	int					width, height;
	bool				vSync;
	
public:
	Window() = delete;
	Window(const std::string& name, int width, int height, int vsync);
	Window operator=(Window copy) = delete;
	//Window& operator=(Window& reference) = default;

	std::weak_ptr<Game> game;
	virtual void		OnKey();
	virtual void		OnMouseMove();
	virtual void		OnMouseClick();
	void				InitGLFW();
	HWND				GetHWND() const;
	GLFWwindow*			GetRenderWindow() const;
	int&				GetWidth();
	int&				GetHeight();
};

