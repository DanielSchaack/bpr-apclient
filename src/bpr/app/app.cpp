#include "app.hpp"

App* App::Instance = nullptr;

App::App(){
    Instance = this;
    guiInstance = new GUI();
}

App::~App()
{
    
    delete guiInstance;
    guiInstance = nullptr;

    Instance = nullptr;
}