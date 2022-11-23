#include <common/ImGuiExt.h>
#include <imgui_internal.h>
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
                first = false;
            else
                ImGui::SameLine(0, spacing);

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
            TooltipLastHoveredItem(tooltip);

        return pressed;
    }

    bool BeginPopupContextItem(const void* ptr_id, ImGuiPopupFlags popup_flags/* = 1*/)
    {
        // this is based on ImGui::BeginPopupContextItem but with a pointer
        // based widget id instead of a string
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (window->SkipItems)
            return false;
        IM_ASSERT(ptr_id);
        ImGuiID id = ImGui::GetID(ptr_id);
        int mouse_button = (popup_flags & ImGuiPopupFlags_MouseButtonMask_);
        if (ImGui::IsMouseReleased(mouse_button) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
            ImGui::OpenPopupEx(id, popup_flags);
        return ImGui::BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    }
}
