#pragma once
#include "../Engine/Game.h"
#include <string>
#include "../Engine/Shader.h"
#include <memory>
#include "../Engine/Mesh.h"
#include "../Engine/Camera.h"
#include "../Engine/Events.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Raycast.h"

class TicScene : public Game
{
public:
	TicScene() = delete;
	TicScene(std::string name, int width, int height, bool vSync);
	virtual void OnKeyPressed(KeyEvent& e) override;
	virtual void OnKeyReleased(KeyEvent& e) override;
	virtual void OnUpdate(UpdateEvent& e) override;
	virtual int	 OnLoad() override;
	virtual void OnRender() override;

private:
	std::shared_ptr<Shader> simpleShader;
	std::unique_ptr<Mesh> quad;
	Bounding::Box box;
	Camera camera;
};

