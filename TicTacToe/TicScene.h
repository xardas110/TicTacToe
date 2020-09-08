#pragma once
#include "../Engine/Game.h"
#include <string>
#include "../Engine/Shader.h"
#include <memory>
class TicScene : public Game
{
public:
	TicScene() = delete;
	TicScene(std::string name, int width, int height, bool vSync);
	virtual int OnLoad() override;
	virtual void OnRender() override;

private:
	std::shared_ptr<Shader> simpleShader;
};

