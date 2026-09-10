#pragma once

#include <windows.h>

class GUI
{
    public:
        GUI();
        ~GUI();
        static bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
};