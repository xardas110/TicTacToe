#include "TicScene.h"

TicScene::TicScene(std::string name, int width, int height, bool vSync)
	:Game(name, width, height)
{
}

int TicScene::OnLoad()
{
	simpleShader = std::shared_ptr<Shader>(new Shader("Shaders/Box.vs", "Shaders/Box.fs"));
	simpleShader->Use();
	return 1;
}

void TicScene::OnRender()
{
	simpleShader->Use();
	glDrawArrays(GL_QUAD_STRIP, 0, 3);
}
