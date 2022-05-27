#include <common/ImGuiExt.h>
#include <fmt/format.h>

namespace ImGuiExt
{
    void TextColored(ColoredTextView text)
    {
        const auto& [textStr, textColor] = text;
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::TextUnformatted(textStr.data(), textStr.data() + textStr.size());
        ImGui::PopStyleColor();
    }

    void TextColored(const ColoredText& text)
    {
        TextColored(ColoredTextView{ text.first, text.second });
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

    void Hyperlink(const std::string& uri)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

        if (ImGui::Button(uri.data()))
        {
            #if defined(_WIN32)
            int result = std::system(fmt::format("cmd /c start {}", uri).data());
            #elif defined(__linux__)
            int result = std::system(fmt::format("xdg-open {}", uri).data());
            #endif
            (void)result;
        }

        ImGui::PopStyleColor(2);
    }

    bool ToggleButton(std::string_view label, bool& b, std::string_view tooltip/* = {}*/)
    {
        bool pressed = false;

        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[b ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
        if (ImGui::Button(label.data()))
        {
            b = !b;
            pressed = true;
        }
        ImGui::PopStyleColor();

        if (!tooltip.empty())
        {
            TooltipLastHoveredItem(tooltip);
        }

        return pressed;
    }
}
