#pragma once
#include "window.hpp"

class ConfigWindow : public Window {
public:
	ConfigWindow() : Window("Config") {
		isVisible = false;
	};
    void ToggleVisibility() override;
    void Draw(int outerWidth, int outerHeight, float uiScale) override;
private:
    bool positionApplied = false;
};

