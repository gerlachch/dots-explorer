#pragma once
#include <utility>
#include <string>
#include <string_view>
#include <vector>
#include <imgui.h>

namespace ImGuiExt
{
    using ColoredTextView = std::pair<std::string_view, const ImVec4&>;
    using ColoredText = std::pair<std::string, ImVec4>;

    void TextColored(ColoredTextView text);
    void TextColored(const ColoredText& text);
    void TextColored(const std::vector<ColoredText>& text, float spacing = -1.0f);

    void TooltipLastHoveredItem(std::string_view text);
}
