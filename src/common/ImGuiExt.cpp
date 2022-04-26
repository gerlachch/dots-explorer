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

    void TextColored(const std::vector<ColoredText>& text)
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
                ImGui::SameLine();
            }

            TextColored(textPart);
        }
    }
}
