#include "../Engine/Application.h"
#include "TicScene.h"

int main()
{
    {
        std::shared_ptr<TicScene> demo = std::make_shared<TicScene>("TiCTacoToe", 1920, 1030, false);
        Application::Create(demo);
        Application::Get().Run(demo);
    }
    Application::Destroy();
}