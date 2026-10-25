#pragma once
#include "window.hpp"

class TrackerWindow : public Window {
public:
	TrackerWindow() : Window("Tracker") {
		isVisible = false;
	};
    void ToggleVisibility() override;
    void Draw(int outerWidth, int outerHeight, float uiScale) override;
private:
    bool positionApplied = false;
};
