#pragma once
#include <string>
#include <vector>

enum class GoalStatus { Met, Achievable, NotMet, NotAGoal };

struct GoalRow {
    std::string label, done, required, available;
    GoalStatus  status;
};

struct GoalSection {
    std::string          title;
    std::vector<GoalRow> rows;
};

std::vector<GoalSection> BuildGoalSections();


struct TrackerRow {
    std::string name;
    int         done, available, total;
    GoalStatus  status;
};

struct TrackerSection {
    std::string            title;
    std::vector<TrackerRow> rows;
};

std::vector<TrackerSection> BuildTrackerSections();

void DrawStatusIcon(GoalStatus s);
void CellCenter(const std::string& s);
