#pragma once
#include "bpr/net/net_bridge.hpp"
#include "bpr/net/ap_net.hpp"
#include "bpr/core/ap/ap_state.hpp"
#include "bpr/ui/gui.hpp"
#include <windows.h>
#include <thread>

class App
{
    public:
        App();
        ~App();
        
        static App* Instance;
   
        GUI& Gui()
        {
            return gui_;
        }

        ApState& State()
        {
            return state_;
        }

        ArchepelagoNet& Network()
        {
            return network_;
        }
        
        static bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static void Render();
        static void GameThread(void* gameActionQueue);
    private:
        NetworkBridge bridge_;
        ApState state_;
        ArchepelagoNet network_;
        GUI gui_;
        std::thread network_thread_;
};
