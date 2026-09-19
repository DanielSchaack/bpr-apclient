#include "app.hpp"

App* App::Instance = nullptr;

App::App(): state_(bridge_),
          network_(bridge_),
          gui_(){
    Instance = this;
    
    network_thread_ = std::thread([this]()
    {
        network_.Run();
    });

    
}

App::~App()
{
    network_.Stop();

    if (network_thread_.joinable())
    {
        network_thread_.join();
    }

    Instance = nullptr;
}

bool App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if (App::Instance != nullptr){
        return App::Instance->Gui().WndProc(hWnd, msg, wParam, lParam);
    }
    return false;
}
void App::Render(){
    if (App::Instance != nullptr){
        App::Instance->Gui().Render();
    }
}

void App::GameThread(){
    if (App::Instance != nullptr){
        App::Instance->State().Update();
    }
}