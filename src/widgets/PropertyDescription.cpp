#include <widgets/PropertyDescription.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyDescription::PropertyDescription(const dots::type::PropertyPath& path)
{
    const dots::type::PropertyDescriptor& descriptor = path.destination();

    m_parts.emplace_back(
        fmt::format("{: >{}}{: >2}:", "", 2 * (path.elements().size() - 1), descriptor.tag()),
        ColorThemeActive.IntegralType
    );

    if (descriptor.isKey())
    {
        m_parts.emplace_back("[key]", ColorThemeActive.Keyword);
    }

    m_parts.emplace_back(
        descriptor.valueDescriptor().name(),
        descriptor.valueDescriptor().isFundamentalType() ? ColorThemeActive.Keyword : ColorThemeActive.UserType
    );

    m_parts.emplace_back(descriptor.name(), ImGui::GetStyle().Colors[ImGuiCol_Text]);
}

void PropertyDescription::render()
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
