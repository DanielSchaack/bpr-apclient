#include "gui.hpp"
#include <imgui.h>
#include <iostream>
#include <d3d11.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

GUI::GUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    HWND windowHandle = *reinterpret_cast<HWND*>(0x0139815C);
    ID3D11Device* d3d11Device = *reinterpret_cast<ID3D11Device**>(0x01485BF8);
    ID3D11DeviceContext* d3d11DeviceContext = *reinterpret_cast<ID3D11DeviceContext**>(0x01485ECC); 
    ImGui_ImplWin32_Init(windowHandle);
    ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);

    SetClassLongPtrA(windowHandle, GCLP_HCURSOR, NULL);
} 
GUI::~GUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    std::cout << "Unloaded ImGui manager.";
}


// extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool GUI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_F3) {
        std::cout << "F3 Was Pressed";
    }


    // if (API::DrawingGUI())
    //     if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    //         return true;
    return false;
}