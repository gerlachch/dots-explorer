#pragma once
#include <utility>
#include <string>
#include <vector>
#include <imgui.h>

namespace ImGuiExt
{
    using ColoredText = std::pair<std::string, ImVec4>;

    void TextColored(const ColoredText& text);
    void TextColored(const std::vector<ColoredText>& text, float spacing = -1.0f);

    void TooltipLastHoveredItem(std::string_view text);
}
