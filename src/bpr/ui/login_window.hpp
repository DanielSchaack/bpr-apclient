#pragma once
#include "window.hpp"


// #define STRINGIFY_IMPL(x) #x
// #define STRINGIFY(x) STRINGIFY_IMPL(x)

static constexpr const char* VERSION = BPRAP_RELEASE_VERSION;

class LoginWindow : public Window {
public:
    LoginWindow() : Window(std::string("Login v") + VERSION) {
        isVisible = true;
    };
    void ToggleVisibility() override;
    void Draw(int outerWidth, int outerHeight, float uiScale) override;
    void SetMessage(std::string);
    char server[128] = "localhost:38281"; //archipelago.gg:";
    char password[128] = "";
    char slot[128] = "";
    char id[128] = "";
    char mptr[128] = "";
    char mstate[128] = "";
private:
    std::string message;
    std::string mPointer;
};

