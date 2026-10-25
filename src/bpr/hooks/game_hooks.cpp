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

    RedirectSave::Install();
    WaitForConnection::Install();

    DeathLink::Install();
    AlwaysWrecked::Install();
    Filler::Install();
    HudNotify::Install();
    MinimapFix::Install();

    DisableTrigger::Install();
    DisableEventStart::Install();
    DetectBreakable::Install();
    DetectRoadRules::Install();
    DetectIslandRoadRules::Install();
    DetectTakedown::Install();
    DetectDriveThru::Install();
    DetectActiveCar::Install();
    DetectBikeEvent::Install();

    CarUnlockControl::Install();

    EventWinLog::Install();
    LicenseUpgradeLog::Install();

    MH_EnableHook(MH_ALL_HOOKS);
}

bool GameHooks::isInGame() noexcept{
    return EnableEvent::IsValidEvent(481382);
}
