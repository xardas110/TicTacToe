#include "../include/glm/gtc/matrix_transform.hpp"
#include "../Engine/Debug.h"
#include "TicScene.h"
#include <iostream>
#include "../Engine/Application.h"

TicScene::TicScene(std::string name, int width, int height, bool vSync)
	:Game(name, width, height), mX(0.f), mY(0.f)
{
}
void TicScene::OnMouseClick(MouseClickEvent& e)
{
	//testing only
	if (e.button == MouseClickEvent::Button::Right && e.state == MouseClickEvent::ButtonState::Pressed)
	{
		if (settings.playerSide == Board::PlayerO)
			settings.playerSide = Board::PlayerX;
		else
			settings.playerSide = Board::PlayerO;
	}
	if (e.button == MouseClickEvent::Button::Left && e.state == MouseClickEvent::ButtonState::Pressed)
	{
		//Transfering x, y pixel cordinades to -1:1
		auto const xHN = ((mX * 2.f) / (float)(win->GetWidth())) - 1.f;
		auto const yHN = 1.f - ((mY * 2.f) / (float)win->GetHeight());
		auto inverseView = glm::inverse(camera.GetViewMatrix());
		auto inverseProj = glm::inverse(camera.GetProjectionMatrix());

		//std::cout << xHN << " " << yHN << std::endl;
		glm::vec4 hn(xHN, yHN, -1.f, 1.f); //z = -1.f because openGL uses a RH coordinate system
		glm::vec4 rayClip = inverseProj * hn;
		rayClip = glm::vec4(rayClip.x, rayClip.y, -1.f, 0.f);

		glm::vec4 rayDir4 = (inverseView * rayClip);
		glm::vec3 radyDir3(rayDir4.x, rayDir4.y, rayDir4.z);
		radyDir3 = glm::normalize(radyDir3);

		RayCast ray(camera.GetPosition(), radyDir3);
		float tMin = -FLT_MAX;
		if (ray.Intersect(board->GetBoundingBox(), tMin))
		{
			glm::vec3 intersectPoint = camera.GetPosition() + (radyDir3 * tMin);
			board->SetTurn(intersectPoint, settings.playerSide);
		}
	}
}

int TicScene::OnLoad()
{
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.SetLookAt(camPos, glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
	camera.SetProjection(45.0f, (float)win->GetWidth() / (float)win->GetHeight(), 0.1f, 100.0f);
	primitiveMaterialShader = std::shared_ptr<Shader>(new Shader("Shaders/PrimitivesMaterial.vs", "Shaders/PrimitivesMaterial.fs"));
	primitiveTextureShader = std::shared_ptr<Shader>(new Shader("Shaders/PrimitivesTextured.vs", "Shaders/PrimitivesTextured.fs"));
	skyboxShader = std::shared_ptr<Shader>(new Shader("Shaders/Skybox.vs", "Shaders/Skybox.fs"));
	settings.playerSide = Board::Player::PlayerO;
	settings.aiSide = Board::Player::PlayerX;
	settings.Ai = std::shared_ptr<AIScripted>(new AIScripted((AIScripted::Player)settings.playerSide, (AIScripted::Player)settings.aiSide));
	settings.gameType = Settings::GameTypes::PVE;
	std::vector<std::string> textures
	{
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_left.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_right.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_up.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_down.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_front.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_back.png"
	};
	
	skybox.Init(textures);
	//board.SetTranslate(glm::vec3(1.f, 1.f, 0.f));//gotta set scaling and translation before board.init because ai runs it's first turn from there, TODO: find a better place for the ai to do its first turn, I don't want to recalculate draw positions for it
	//board.SetScale(glm::vec3(1.25f, 1.25f, 1.f));
	board = std::unique_ptr<Board>(new Board(settings.Ai));
	board->Init();
	if (!Application::CreateNewMenu("Menu", menu, shared_from_this()))
	{ 
		std::cout << "Failed to create menu" << std::endl;
		return 0;
	}
	return 1;
}

void TicScene::OnRender()
{	
	Application::ClearColor(0.f, 0.f, 0.f);
	Application::GetFrambufferSize(win->GetRenderWindow(), &win->GetWidth(), &win->GetHeight());//this function should run on resize, but im too lazy to add everything
	Application::SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Application::SetDepthMask(FALSE);
	Application::SetColorMask(TRUE, TRUE, TRUE, TRUE);

	glm::mat4 model(1.f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 project = camera.GetProjectionMatrix();
	glm::mat4 projectView = project * view;
	glm::mat4 skyboxView = glm::mat3(view);//removing translation vector for skybox
	glm::mat4 skyBoxVP = project * skyboxView;
		
	skyboxShader->Use();
	skyboxShader->BindMat4("MVP", skyBoxVP);
	skybox.Draw(skyboxShader);
	primitiveTextureShader->Use();
	primitiveTextureShader->BindVec3("camPos", camera.GetPosition());
	primitiveMaterialShader->Use();
	primitiveMaterialShader->BindVec3("camPos", camera.GetPosition());
	board->Draw(primitiveMaterialShader, primitiveTextureShader, projectView);
	
	Application::SwapBuffer(win->GetRenderWindow());
}
int TicScene::OnControlInit()
{
	menu->CreateButton("Start new game", (float)menu->GetWidth() * 0.5f, menu->GetHeight() - 100, 200, 50, 200);
	HWND gametype = menu->CreateComboBox("gametype", (float)menu->GetWidth() * 0.5f, 0, 200, 100, 201);
	HWND playerside = menu->CreateComboBox("playerside", (float)menu->GetWidth() * 0.5f, 50, 200, 100, 202);
	HWND difficulty = menu->CreateComboBox("difficultycombo", (float)menu->GetWidth() * 0.5f, 100, 200, 100, 203);
	HWND gametypeText = menu->CreateStaticText("Choose Gametype:", 0, 0, 200, 27, NULL);
	HWND playerSideText = menu->CreateStaticText("Choose Side:", 0, 50, 200, 27, NULL);
	HWND difficultyText = menu->CreateStaticText("Choose Difficulty:", 0, 100, 200, 27, NULL);
	HWND statusText = menu->CreateStaticText("Status", 0, menu->GetHeight() - 100, 200, 50, NULL);
	CHAR options[2][5] =
	{
		"PVP", "PVE"
	};
	CHAR option[16];
	memset(&option, 0, sizeof(option));
	for (int i = 0; i < _countof(options); i++)
	{
		strcpy_s(option, _countof(option), (CHAR*)options[i]);
		SendMessageA(gametype, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)option);
	}
	SendMessageA(gametype, CB_SETCURSEL, (WPARAM)settings.gameType, (LPARAM)0);

	CHAR options1[2][10] =
	{
		"PlayerO", "PlayerX"
	};
	memset(&option, 0, sizeof(option));
	for (int i = 0; i < _countof(options1); i++)
	{
		strcpy_s(option, _countof(option), (CHAR*)options1[i]);
		SendMessageA(playerside, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)option);
	}
	SendMessageA(playerside, CB_SETCURSEL, (WPARAM)settings.playerSide, (LPARAM)0);

	CHAR options2[3][10] =
	{
		"Easy", "Medium", "Hard"
	};
	memset(&option, 0, sizeof(option));
	for (int i = 0; i < _countof(options2); i++)
	{
		strcpy_s(option, _countof(option), (CHAR*)options2[i]);
		SendMessageA(difficulty, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)option);
	}
	SendMessageA(difficulty, CB_SETCURSEL, (WPARAM)settings.difficulty, (LPARAM)0);

	return 1;
}
void TicScene::OnControlPress(WPARAM wParam, LPARAM lParam)
{
	const auto lo = LOWORD(wParam);
	const auto hi = HIWORD(wParam);

	switch (lo)
	{
	case 200://too lazy to add defines for now
	{
		if (hi == BN_CLICKED)
		{
			if (settings.gameType == Settings::GameTypes::PVP)
			{
				board.release();
				board.~unique_ptr();
				board = std::unique_ptr<Board>(new Board(nullptr));
			}
			else
			{
				if (settings.playerSide == Board::Player::PlayerO)
					settings.aiSide = Board::Player::PlayerX;
				else
					settings.aiSide = Board::Player::PlayerO;

				switch (settings.difficulty)
				{
					case Settings::Difficulty::Easy:
					{
						settings.Ai = std::shared_ptr<AIEasy>(new AIEasy((AIScripted::Player)settings.playerSide, (AIScripted::Player)settings.aiSide));
					}
					break;
					case Settings::Difficulty::Medium:
					{
						settings.Ai = std::shared_ptr<AINormal>(new AINormal((AIScripted::Player)settings.playerSide, (AIScripted::Player)settings.aiSide));
					}
					break;
					case Settings::Difficulty::Hard:
					{
						settings.Ai = std::shared_ptr<AIScripted>(new AIScripted((AIScripted::Player)settings.playerSide, (AIScripted::Player)settings.aiSide));
					}
					break;
					default:
						settings.Ai = std::shared_ptr<AIScripted>(new AIScripted((AIScripted::Player)settings.playerSide, (AIScripted::Player)settings.aiSide));
						break;
				}

				board = std::unique_ptr<Board>(new Board(settings.Ai));
			}
			board->Init();
			HWND status = menu->GetControl("Status");
			if (status)
			{
				SetWindowTextA(status, "Status: Game Ongoing");
			}
		}
	}
	break;
	case 201:
	{
		if (hi == CBN_SELCHANGE)
		{
			int itemIndex = SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			std::cout << itemIndex << std::endl;
			settings.gameType = (Settings::GameTypes)itemIndex;
			HWND status = menu->GetControl("Status");
			if (status)
			{
				SetWindowTextA(status, "Start new game for settings to take affect");
			}
		}
		break;
	}
	case 202:
	{
		if (hi == CBN_SELCHANGE)
		{
			int itemIndex = SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			settings.playerSide = (Board::Player)itemIndex;
			HWND status = menu->GetControl("Status");
			if (status)
			{
				SetWindowTextA(status, "Start new game for settings to take affect");
			}
		}
		break;
	}
	case 203:
	{
		if (hi == CBN_SELCHANGE)
		{
			int itemIndex = SendMessageA((HWND)lParam, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			settings.difficulty = (Settings::Difficulty)itemIndex;
			HWND status = menu->GetControl("Status");
			if (status)
			{
				SetWindowTextA(status, "Start new game for settings to take affect");
			}
		}
		break;
	}
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
		camera.AddVelocityX(0.f);
		break;
	case GLFW_KEY_A:
		camera.AddVelocityX(0.f);
		break;
	default:
		break;
	}
}