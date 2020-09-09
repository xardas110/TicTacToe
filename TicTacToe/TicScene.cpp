#include "TicScene.h"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <iostream>
TicScene::TicScene(std::string name, int width, int height, bool vSync)
	:Game(name, width, height)
{
}

void TicScene::OnKeyPressed(KeyEvent& e)
{
	switch (e.key)
	{
	case GLFW_KEY_W:
		camera.AddVelocityY(1.f);
		break;
	case GLFW_KEY_S:
		camera.AddVelocityY(-1.f);
		break;
	case GLFW_KEY_D:
		camera.AddVelocityX(1.f);
		break;
	case GLFW_KEY_A:
		camera.AddVelocityX(-1.f);
		break;
	default:
		break;
	}
}

void TicScene::OnKeyReleased(KeyEvent& e)
{
	switch (e.key)
	{
	case GLFW_KEY_W:
		camera.AddVelocityY(0.f);
		break;
	case GLFW_KEY_S:
		camera.AddVelocityY(0.f);
		break;
	case GLFW_KEY_D:
	{
		camera.AddVelocityX(0.f);
		break;
	}
	case GLFW_KEY_A:
		camera.AddVelocityX(0.f);
		break;
	default:
		break;
	}
}

void TicScene::OnUpdate(UpdateEvent& e)
{
	//std::cout << e.deltaTime << std::endl;
	camera.UpdatePosition(e);
}

int TicScene::OnLoad()
{
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camera.SetLookAt(camPos, glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
	camera.SetProjection(45.0f, (float)win->GetWidth() / (float)win->GetHeight(), 0.1f, 100.0f);
	simpleShader = std::shared_ptr<Shader>(new Shader("Shaders/Box.vs", "Shaders/Box.fs"));
	quad = Mesh::CreateQuad();
	simpleShader->Use();
	return 1;
}

void TicScene::OnRender()
{
	
	glm::mat4 model(1.f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 project = camera.GetProjectionMatrix();
	glm::mat4 MVP =  project * view * model;
	simpleShader->Use();
	simpleShader->BindVec3("color", glm::vec3(1.f, 0.f, 0.f));
	simpleShader->BindMat4("MVP", MVP);
	quad->Draw(GL_TRIANGLES);
}
