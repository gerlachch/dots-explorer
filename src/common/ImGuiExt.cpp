#include <common/ImGuiExt.h>

namespace ImGuiExt
{
    void TextColored(const ColoredText& text)
    {
        const auto& [textStr, textColor] = text;
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::TextUnformatted(textStr.data());
        ImGui::PopStyleColor();
    }

    void TextColored(const std::vector<ColoredText>& text, float spacing/* = -1.0f*/)
    {
        bool first = true;

        for (const ColoredText& textPart : text)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                ImGui::SameLine(0, spacing);
            }

            TextColored(textPart);
        }
    }

    void TooltipLastHoveredItem(std::string_view text)
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(text.data(), text.data() + text.size());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}
