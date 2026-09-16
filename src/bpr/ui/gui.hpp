#pragma once

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
    private:
        std::vector<std::unique_ptr<Window>> windows;
};