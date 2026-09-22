#pragma once

#include "bpr/ui/login_window.hpp"
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
        LoginWindow* login_window;
        InfoWindow* info_window;
        bpr::BroadcastBanner banner_;
    private:
        std::vector<std::unique_ptr<Window>> windows;
        HWND windowHandle;
        void SetInputMode(bool enabled);
        bool imguiInputMode = false;
};