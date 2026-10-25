#pragma once

#include "bpr/ui/config_window.hpp"
#include "bpr/ui/login_window.hpp"
#include "bpr/ui/quicktravel_window.hpp"
#include "bpr/ui/tracker_window.hpp"
#include "broadcast_banner.hpp"
#include "info_window.hpp"
#include "login_window.hpp"
#include "window.hpp"
#include <memory>
#include <vector>
#include <windows.h>

class GUI
{
    public:
        GUI(bpr::BannerQueue &banner_queue);
        ~GUI();
        bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void Render();
        void Initialize();
        TrackerWindow* tracker_window;
        LoginWindow* login_window;
        ConfigWindow* config_window;
        QuickTravelWindow* quicktravel_window;
        InfoWindow* info_window;
        bpr::BroadcastBanner banner_;
    private:
        std::vector<std::unique_ptr<Window>> windows;
        HWND windowHandle;
        void SetInputMode(bool enabled);
        void UpdateInputMode();
        bool imguiInputMode = true;
};
