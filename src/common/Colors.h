#pragma once
#include <imgui.h>

struct Colors
{
    ImVec4 Success;
    ImVec4 Error;
    ImVec4 Pending;

    ImVec4 Keyword;
    ImVec4 UserType;
    ImVec4 StringType;
    ImVec4 EnumType;
    ImVec4 IntegralType;
};

inline constexpr Colors ColorThemeDark{
    ImVec4{ 0.00f, 1.00f, 0.00f, 1.00f },
    ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f },
    ImVec4{ 1.00f, 0.75f, 0.00f, 1.00f },

    ImVec4{ 0.34f, 0.61f, 0.84f, 1.00f },
    ImVec4{ 0.31f, 0.79f, 0.69f, 1.00f },
    ImVec4{ 0.91f, 0.79f, 0.73f, 1.00f },
    ImVec4{ 0.75f, 0.72f, 1.00f, 1.00f },
    ImVec4{ 0.72f, 0.84f, 0.64f, 1.00f }
};

inline constexpr Colors ColorThemeActive = ColorThemeDark;
