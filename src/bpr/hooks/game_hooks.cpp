#include "game_hooks.hpp"
#include "MinHook.h"
#include "structure/detours.hpp"
#include <iostream>

void GameHooks::Init(){
    MH_Uninitialize();
    MH_Initialize();
    WndProc::Install();
    MH_EnableHook(MH_ALL_HOOKS);
}