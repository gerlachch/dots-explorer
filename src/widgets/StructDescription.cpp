#include <widgets/StructDescription.h>
#include <common/Colors.h>

StructDescription::StructDescription(const dots::type::StructDescriptor<>& descriptor)
{
    m_parts.emplace_back("struct", ColorThemeActive.Keyword);
    m_parts.emplace_back(descriptor.name(), ColorThemeActive.UserType);

    if (uint8_t flags = descriptor.flags(); flags > 1)
    {
        std::string part = "[";

        if (!descriptor.cached())
        {
            part += "cached=false, ";
        }

        if (descriptor.cleanup())
        {
            part += "cleanup, ";
        }

        if (descriptor.local())
        {
            part += "local, ";
        }

        if (descriptor.persistent())
        {
            part += "persistent, ";
        }

        if (descriptor.internal())
        {
            part += "internal, ";
        }

        if (descriptor.substructOnly())
        {
            part += "substruct_only, ";
        }

        part.erase(part.size() - 2);
        part += ']';

        m_parts.emplace_back(std::move(part), ColorThemeActive.Keyword);
    }
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
