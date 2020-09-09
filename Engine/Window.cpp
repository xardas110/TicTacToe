#include "Window.h"
#include "Game.h"

Window::Window(const std::string& name, int width, int height, int vsync)
:name(name), width(width), height(height), vSync(vsync)
{
}

void Window::OnKeyPressed(KeyEvent& e)
{
    if (auto pGame = game.lock())
    {
        pGame->OnKeyPressed(e);
    }
}

void Window::OnKeyReleased(KeyEvent& e)
{
    if (auto pGame = game.lock())
    {
        pGame->OnKeyReleased(e);
    }
}

void Window::OnMouseMove()
{
    if (auto pGame = game.lock())
        pGame->OnMouseMove();
}

void Window::OnMouseClick()
{
    if (auto pGame = game.lock())
        pGame->OnMouseClick();
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
