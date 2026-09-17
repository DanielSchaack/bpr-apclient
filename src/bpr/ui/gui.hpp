#pragma once

#include "login_window.hpp"
#include "window.hpp"
#include <memory>
#include <vector>
#include <windows.h>

class GUI
{
    public:
        GUI();
        ~GUI();
        bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void Render();
        void Initialize();
        std::unique_ptr<Window> login_window;
    private:
        std::vector<std::unique_ptr<Window>> windows;
        HWND windowHandle;
        void SetInputMode(bool enabled);
        bool imguiInputMode = false;
};