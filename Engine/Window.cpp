#include "Window.h"
#include "Game.h"

Window::Window(const std::string& name, int width, int height, int vsync)
:name(name), width(width), height(height), vSync(vsync)
{
}

void Window::OnKeyPressed(KeyEvent& e)
{
    if (auto Game = game.lock())
    {
        Game->OnKeyPressed(e);
    }
}

void Window::OnKeyReleased(KeyEvent& e)
{
    if (auto Game = game.lock())
    {
        Game->OnKeyReleased(e);
    }
}

void Window::OnMouseMove(MouseMoveEvent& e)
{
    if (auto Game = game.lock())
        Game->OnMouseMove(e);
}

void Window::OnMouseClick(MouseClickEvent& e)
{
    if (auto Game = game.lock())
        Game->OnMouseClick(e);
}

void Window::InitGLFW()
{
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL); 
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}

HWND Window::GetHWND() const
{
    return hwnd;
}

GLFWwindow* Window::GetRenderWindow() const
{
    return window;
}

int& Window::GetWidth()
{
    return width;
}

int& Window::GetHeight()
{
    return height;
}
