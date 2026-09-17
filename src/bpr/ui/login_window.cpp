#include "login_window.hpp"
#include <functional>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <string>
#include "../app/app.hpp"


void LoginWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void LoginWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    // Draw login window
    ImGui::Begin(name.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::InputText("Server", server, IM_ARRAYSIZE(server));
    ImGui::InputText("Password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
    ImGui::InputText("Slot Name", slot, IM_ARRAYSIZE(slot));

    if (App::Instance->State().isDisconnected()) {
        if (ImGui::Button("Connect")) {
            if (strlen(server) > 0 && strlen(slot) > 0) {
                App::Instance->State().Connect(server, slot, password);
                SetMessage("Connecting to " + std::string(server) + "...");
            }
            else {
                SetMessage("Please enter server and slot name");
            }
        }
    }
    else {
        if (ImGui::Button("Disconnect")) {
            App::Instance->State().Disconnect();
            SetMessage("");
        }
    }


    ImGui::TextWrapped("%s", message.c_str());

    ImGui::End();
}


void LoginWindow::SetMessage(std::string newMessage) {
    message = newMessage;
}
