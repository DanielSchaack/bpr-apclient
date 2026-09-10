#pragma once
#include "bpr/ui/gui.hpp"
#include <Windows.h>

class App
{
    public:
        App();
        ~App();
        
        static App* Instance;
        GUI* guiInstance = nullptr;
};