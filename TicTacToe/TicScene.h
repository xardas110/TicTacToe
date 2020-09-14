#pragma once
#include "../Engine/Game.h"
#include "../Engine/Shader.h"
#include "../Engine/Mesh.h"
#include "../Engine/Camera.h"
#include "../Engine/Events.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Raycast.h"
#include "../Engine/Skybox.h"
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

private:
	std::shared_ptr<Shader> simpleShader;
	std::shared_ptr<Shader> skyboxShader;
	Skybox skybox;
	Camera camera;
	float mX, mY;
	Board board;
};

