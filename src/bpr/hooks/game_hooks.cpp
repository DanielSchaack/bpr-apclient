#include "game_hooks.hpp"
#include "MinHook.h"
#include "function/detours.hpp"
#include "structure/detours.hpp"

void GameHooks::Init(){
    MH_Uninitialize();
    MH_Initialize();
    WndProc::Install();
    logEvent::Install();
    DisableTrigger::Install();
    DisableEventStart::Install();
    DetectBreakable::Install();
    Render::Install();
    MH_EnableHook(MH_ALL_HOOKS);
}