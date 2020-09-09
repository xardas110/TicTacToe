#include "TicScene.h"

TicScene::TicScene(std::string name, int width, int height, bool vSync)
	:Game(name, width, height)
{
}

int TicScene::OnLoad()
{
	simpleShader = std::shared_ptr<Shader>(new Shader("Shaders/Box.vs", "Shaders/Box.fs"));
	quad = Mesh::CreateGrid(3, 3, 0.4f, 0.4f);
	simpleShader->Use();
	return 1;
}

void TicScene::OnRender()
{
	simpleShader->Use();
	quad->Draw(GL_LINES);
}
