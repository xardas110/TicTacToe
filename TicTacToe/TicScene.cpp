#include "TicScene.h"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include "../Engine/Debug.h"


TicScene::TicScene(std::string name, int width, int height, bool vSync)
	:Game(name, width, height), mX(0.f), mY(0.f)
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
	camera.UpdatePosition(e.deltaTime);
	
}

void TicScene::OnMouseMove(MouseMoveEvent& e)
{	
	mX = (float)e.xPos; mY = (float)e.yPos;
}

void TicScene::OnMouseClick(MouseClickEvent& e)
{
	//testing only
	static Board::Player player = Board::PlayerO;
	if (e.button == MouseClickEvent::Button::Right && e.state == MouseClickEvent::ButtonState::Pressed)
	{
		if (player == Board::PlayerO)
			player = Board::PlayerX;
		else
			player = Board::PlayerO;
	}
	if (e.button == MouseClickEvent::Button::Left && e.state == MouseClickEvent::ButtonState::Pressed)
	{ 
		//Transfering x, y pixel cordinades to -1:1
		auto const xHN = ((mX * 2.f) / (float)(win->GetWidth())) - 1.f;
		auto const yHN = 1.f - ((mY * 2.f) / (float)win->GetHeight());
		auto inverseView = glm::inverse(camera.GetViewMatrix());
		auto inverseProj = glm::inverse(camera.GetProjectionMatrix());

		//std::cout << xHN << " " << yHN << std::endl;
		glm::vec4 hn(xHN, yHN, -1.f, 1.f); //z = -1.f because in openGL -z is forward by default
		glm::vec4 rayClip = inverseProj * hn;
		rayClip = glm::vec4(rayClip.x, rayClip.y, -1.f, 0.f);

		glm::vec4 rayDir4 = (inverseView * rayClip);
		glm::vec3 radyDir3(rayDir4.x, rayDir4.y, rayDir4.z);
		radyDir3 = glm::normalize(radyDir3);

		RayCast ray(camera.GetPosition(), radyDir3);
		float tMin = -FLT_MAX;
		if (ray.Intersect(board.GetBoundingBox(), tMin))
		{
			glm::vec3 intersectPoint = camera.GetPosition() + (radyDir3 * tMin);
			board.SetTile(intersectPoint, player);
		}
	}
}

int TicScene::OnLoad()
{
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.SetLookAt(camPos, glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
	camera.SetProjection(45.0f, (float)win->GetWidth() / (float)win->GetHeight(), 0.1f, 100.0f);
	simpleShader = std::shared_ptr<Shader>(new Shader("Shaders/Primitives.vs", "Shaders/Primitives.fs"));
	board.Init();
	simpleShader->Use();
	//board.SetTranslate(glm::vec3(1.f, 1.f, 0.f));
	//board.SetScale(glm::vec3(2.f, 2.f, 1.f)); //TODO add player scaling to fit this
	return 1;
}

void TicScene::OnRender()
{	
	glm::mat4 model(1.f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 project = camera.GetProjectionMatrix();
	glm::mat4 projectView = project * view;
	glm::mat4 MVP = projectView * board.GetWorldSpace();
	simpleShader->Use();
	simpleShader->BindVec3("color", glm::vec3(1.f, 0.f, 0.f));
	simpleShader->BindMat4("MVP", MVP);
	board.Draw(simpleShader, projectView);
}
