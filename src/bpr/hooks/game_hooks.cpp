#include "game_hooks.hpp"
#include "MinHook.h"
#include "function/detours.hpp"
#include "structure/detours.hpp"

void GameHooks::Init(){
    MH_Uninitialize();
    MH_Initialize();
    WndProc::Install();
    Render::Install();
    GameLoop::Install();
    DeathLink::Install();
    RedirectSave::Install();
    WaitForConnection::Install();

    // logEvent::Install();
    DisableTrigger::Install();
    DisableEventStart::Install();
    DetectBreakable::Install();
    
    // CarUnlockControl::Install();
    
    EventWinLog::Install();
    LicenseUpgradeLog::Install();
    
    MH_EnableHook(MH_ALL_HOOKS);
}