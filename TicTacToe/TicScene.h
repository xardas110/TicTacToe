#pragma once
#include "../Engine/Game.h"
#include "../Engine/Shader.h"
#include "../Engine/Mesh.h"
#include "../Engine/Camera.h"
#include "../Engine/Events.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Raycast.h"
#include "../Engine/Skybox.h"
#include "../Engine/Texture.h"
#include "../Engine/Menu.h"
#include "Settings.h"
#include "Board.h"
#include <memory>
#include <string>
class TicScene : public Game
{
public:
	TicScene() = delete;
	TicScene(std::string name, int width, int height, bool vSync);
	virtual void OnKeyPressed(KeyEvent& e) override;
	virtual void OnKeyReleased(KeyEvent& e) override;
	virtual void OnUpdate(UpdateEvent& e) override;
	virtual void OnMouseMove(MouseMoveEvent& e) override;
	virtual void OnMouseClick(MouseClickEvent& e) override;
	virtual int	 OnLoad() override;
	virtual void OnRender() override;
	virtual int  OnControlInit() override;
	virtual void OnControlPress(WPARAM wParam, LPARAM lParam) override;

private:
	std::shared_ptr<Shader> primitiveMaterialShader;
	std::shared_ptr<Shader> primitiveTextureShader;
	std::shared_ptr<Shader> skyboxShader;
	std::shared_ptr<Texture>testTex;
	std::shared_ptr<Menu>	menu;//Can't be unique ptr cuz application needs access to the inherited window ptr
	Skybox skybox;
	Camera camera;
	float mX, mY;
	std::unique_ptr<Board> board;
	Settings settings;
};

