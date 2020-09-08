#include "Application.h"
#include <iostream>
#include <memory>
#include <map>
using WindowMap = std::map<std::string, std::shared_ptr<Window>>;
static Application* gs_S = nullptr;
WindowMap gs_WinMap{};
static void error_callback(int error, const char* description)
{
	std::fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	gs_S->callWindow->OnKey();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	gs_S->callWindow->OnMouseMove();
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	gs_S->callWindow->OnMouseClick();
}

void Application::Create()
{
	if (!gs_S)
	{
		gs_S = new Application();
		gs_S->Init();
	}
}

Application& Application::Get()
{
	return *gs_S;
}

int Application::Run(std::shared_ptr<Game> g)
{
	if (g->Init() != 1) return -1;
	if (SetGLFWCallback(g->win)!= 1) return -1;
	while (!glfwWindowShouldClose(g->win->GetRenderWindow()))
	{
		g->OnUpdate();
		glfwGetFramebufferSize(g->win->GetRenderWindow(), &g->win->GetWidth(), &g->win->GetHeight());	
		g->OnRender();
		glfwSwapBuffers(g->win->GetRenderWindow());
		glfwPollEvents();
	}
}

std::shared_ptr<Window> Application::CreateRenderWindow(const std::string name, int width, int height, bool vsync)
{
	WindowMap::iterator iter = gs_WinMap.find(name);
	if (iter != gs_WinMap.end())
		return iter->second;
	std::shared_ptr<Window> renderWindow = std::make_shared<Window>(name, width, height, false);
	renderWindow->InitGLFW();
	gs_WinMap.insert(WindowMap::value_type(name, renderWindow));
	return renderWindow;
}

int Application::SetGLFWCallback(std::shared_ptr<Window> w)
{
	gs_S->callWindow = w;
	glfwSetKeyCallback(w->GetRenderWindow(), key_callback);
	glfwSetCursorPosCallback(w->GetRenderWindow(), mouse_callback);
	glfwSetMouseButtonCallback(w->GetRenderWindow(), mouse_button_callback);
	return 1;
}

void Application::Destroy()
{
	WindowMap::iterator iter = gs_WinMap.begin();
	if (iter != gs_WinMap.end())
	{
		glfwDestroyWindow(iter->second->GetRenderWindow());
	}
	
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void Application::Init()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}
