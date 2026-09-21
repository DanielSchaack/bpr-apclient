#include "gui.hpp"
#include <imgui.h>
#include <iostream>
#include <d3d11.h>
#include "bpr/app/app.hpp"
#include "bpr/hooks/game_hooks.hpp"
#include "broadcast_banner.hpp"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "login_window.hpp"
#include "bpr/hooks/function/detours.hpp"

GUI::GUI(bpr::BannerQueue &banner_queue):banner_( banner_queue)
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

    // SetClassLongPtrA(GUI::windowHandle, GCLP_HCURSOR, NULL);

    auto login = std::make_unique<LoginWindow>();
    login_window = login.get();
    windows.push_back(std::move(login));
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
    if (msg == WM_KEYDOWN && wParam == VK_F1)
    {
        SetInputMode(!imguiInputMode);
        return false;
    }

    if (msg == WM_KEYDOWN && wParam == VK_F3)
    {
        GameHooks::PrintCurrentBoostLevel();
        GameHooks::PrintCurrentDeformation();
        return false;
    }

    if (!imguiInputMode)
        return true;
    
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    const ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        switch (msg)
        {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEWHEEL:
            return false;
        }
    }

    if (io.WantCaptureKeyboard)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_MENUCHAR:
        case WM_COMMAND:
            return false;
        }
    }

    return true;
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

    banner_.draw();
    for (auto& window : windows) { window.get()->Draw(windowWidth, windowHeight, uiScale); }


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUI::SetInputMode(bool enabled)
{
    imguiInputMode = enabled;

    ImGuiIO& io = ImGui::GetIO();

    // ImGui draws its own cursor only while input mode is enabled.
    io.MouseDrawCursor = enabled;

    if (enabled)
    {
        // Release the game's mouse clipping.
        ClipCursor(nullptr);
    }
}