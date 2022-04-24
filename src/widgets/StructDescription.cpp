#include <widgets/StructDescription.h>
#include <common/Colors.h>

StructDescription::StructDescription(const dots::type::StructDescriptor<>& descriptor)
{
    m_parts.emplace_back("struct", ColorThemeActive.Keyword);
    m_parts.emplace_back(descriptor.name(), ColorThemeActive.UserType);
}

void StructDescription::render()
{
    bool first = true;

    for (const auto& [part, color] : m_parts)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            ImGui::SameLine();
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(part.data());
        ImGui::PopStyleColor();
    }
}
