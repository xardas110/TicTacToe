#include <Precompiled.h>
#include "Application.h"
#include <map>
#include <iostream>


using WindowMap = std::map<std::string, std::shared_ptr<Window>>;
static Application* gs_S = nullptr;
double Application::lastFrame = glfwGetTime();
double Application::totalTime = 0.0;
uint64_t Application::frameNr = 0ULL;
WindowMap gs_WinMap{};
static void error_callback(int error, const char* description)
{
	std::fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		KeyEvent eArgs(key, scancode, (KeyEvent::KeyState)GLFW_PRESS, mods);
		Application::Get().callWindow->OnKeyPressed(eArgs);
	}
	else if (action == GLFW_RELEASE)
	{
		KeyEvent eArgs(key, scancode, (KeyEvent::KeyState)GLFW_RELEASE, mods);
		gs_S->callWindow->OnKeyReleased(eArgs);
	}
}
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	MouseMoveEvent e(xpos, ypos);
	gs_S->callWindow->OnMouseMove(e);
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	MouseClickEvent e(button, action, mods);
	gs_S->callWindow->OnMouseClick(e);
}

void Application::Create(std::shared_ptr<Game> g)
{
	if (!gs_S)
	{
		gs_S = new Application();
		gs_S->Init(g);
	}
}

Application& Application::Get()
{
	return *gs_S;
}

int Application::Run(std::shared_ptr<Game> g)
{
	//if (g->Init() != 1) return -1;
	if (SetGLFWCallback(g->win)!= 1) return -2;
	if (g->OnLoad() != 1) return -3;
	while (!glfwWindowShouldClose(g->win->GetRenderWindow()))
	{
		const auto currentTime = glfwGetTime();
		const auto deltaTime = currentTime - Application::lastFrame;	

		Application::totalTime += deltaTime;
		Application::lastFrame = currentTime;
		
		UpdateEvent e(deltaTime, Application::totalTime, Application::frameNr);
		g->OnUpdate(e);

		glfwGetFramebufferSize(g->win->GetRenderWindow(), &g->win->GetWidth(), &g->win->GetHeight());
		glViewport(0, 0, g->win->GetWidth(), g->win->GetHeight());
		glClearColor(0.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		g->OnRender();
		glfwSwapBuffers(g->win->GetRenderWindow());
		glfwPollEvents();
		Application::frameNr++;
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

unsigned int Application::LoadCubeMapTextures(std::vector<std::string> textures)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Application::Init(std::shared_ptr<Game> g)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	g->Init();
	glfwMakeContextCurrent(g->win->GetRenderWindow());
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glfwSwapInterval(1);
}
