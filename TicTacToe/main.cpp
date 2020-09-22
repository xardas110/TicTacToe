#include "../Engine/Application.h"
#include "../Engine/CMD.h"
#include "TicScene.h"
#include <future>

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
#define DEBUG   //Leaving this on for now
#ifdef DEBUG
    RedirectIOToConsole();
#endif // DEBUG

    {
        std::shared_ptr<TicScene> TicTacToe = std::make_shared<TicScene>("TiCTacSpace", 1280, 720, false);
        Application::Create(hInstance);
        Application::Get().Run(TicTacToe);//run doesnt need a parameter, TODO: Add weakptr inside application class and remove parameter from Run()
    }
    Application::Destroy();
}