#include <widgets/PropertyDescription.h>
#include <fmt/format.h>
#include <common/Colors.h>

PropertyDescription::PropertyDescription(const dots::type::PropertyPath& path) :
    m_valueQuoted(false),
    m_propertyPath{ path }
{
    const dots::type::PropertyDescriptor& descriptor = path.destination();

    // create description parts
    {
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

    // determine value color
    {
        switch (descriptor.valueDescriptor().type())
        {
            case dots::type::Type::boolean:
                m_valueColor = ColorThemeActive.Keyword;
                break;
            case dots::type::Type::string:
                m_valueColor = ColorThemeActive.StringType;
                m_valueQuoted = true;
                break;
            case dots::type::Type::Enum:
                m_valueColor = ColorThemeActive.EnumType;
                break;
            case dots::type::Type::int8:
            case dots::type::Type::uint8:
            case dots::type::Type::int16:
            case dots::type::Type::uint16:
            case dots::type::Type::int32:
            case dots::type::Type::uint32:
            case dots::type::Type::int64:
            case dots::type::Type::uint64:
            case dots::type::Type::float32:
            case dots::type::Type::float64:
            case dots::type::Type::property_set:
            case dots::type::Type::timepoint:
            case dots::type::Type::steady_timepoint:
            case dots::type::Type::duration:
            case dots::type::Type::uuid:
            case dots::type::Type::Vector:
            case dots::type::Type::Struct:
            default:
                m_valueColor = ColorThemeActive.IntegralType;
                break;
        }
    }
}

const dots::type::PropertyPath& PropertyDescription::propertyPath() const
{
    return m_propertyPath;
}

const ImVec4& PropertyDescription::valueColor() const
{
    return m_valueColor;
}

bool PropertyDescription::valueQuoted() const
{
    return m_valueQuoted;
}

void PropertyDescription::render() const
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
