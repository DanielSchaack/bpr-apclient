#pragma once
#include <cstdint>
#include "window.hpp"

class QuickTravelWindow : public Window {
public:
	QuickTravelWindow() : Window("QuickTravel") {
		isVisible = false;
	};
    void ToggleVisibility() override;
    void Draw(int outerWidth, int outerHeight, float uiScale) override;

private:
    uint64_t selectedId = 0; // 0 = nothing selected
    bool positionApplied = false;
};
