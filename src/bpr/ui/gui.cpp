#include "gui.hpp"
#include <imgui.h>
#include <iostream>
#include <d3d11.h>
#include "bpr/app/app.hpp"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "login_window.hpp"

GUI::GUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.3f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    GUI::windowHandle = *reinterpret_cast<HWND*>(0x0139815C);
    ID3D11Device* d3d11Device = *reinterpret_cast<ID3D11Device**>(0x01485BF8);
    ID3D11DeviceContext* d3d11DeviceContext = *reinterpret_cast<ID3D11DeviceContext**>(0x01485ECC); 
    ImGui_ImplWin32_Init(GUI::windowHandle);
    ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);

    SetClassLongPtrA(GUI::windowHandle, GCLP_HCURSOR, NULL);

    
    windows.push_back(std::make_unique<LoginWindow>());
} 

GUI::~GUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    std::cout << "Unloaded ImGui manager.";
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool GUI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_F3) {
        App::Instance->State().Connect("localhost:38321", "Fyre", "");
    }


    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return false;
}

void GUI::Render(){
    RECT rect;
    if (!GetClientRect(GUI::windowHandle, &rect)) {
        return;
    }
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    const int baseWidth = 1920;
    const int baseHeight = 1080;
    float widthScale = static_cast<float>(windowWidth) / baseWidth;
    float heightScale = static_cast<float>(windowHeight) / baseHeight;
    float uiScale = (((widthScale) < (heightScale)) ? (widthScale) : (heightScale));

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    for (auto& window : windows) { window.get()->Draw(windowWidth, windowHeight, uiScale); }


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}