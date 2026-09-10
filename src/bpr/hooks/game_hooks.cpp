#include "game_hooks.hpp"
#include "MinHook.h"
#include "structure/detours.hpp"
#include <iostream>

void GameHooks::Init(){
    MH_Uninitialize();
    MH_Initialize();
    std::cout << "Finished INIT MH" << std::endl;
    WndProc::Install();
    MH_EnableHook(MH_ALL_HOOKS);
    std::cout << "Finished MH_ALL_HOOKS" << std::endl;
}