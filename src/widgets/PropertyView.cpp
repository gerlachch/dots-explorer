#include <widgets/PropertyView.h>
#include <common/Colors.h>

PropertyView::PropertyView(const dots::type::ProxyProperty<>& property) :
    m_isSelected(false),
    m_valueQuoted(false),
    m_property{ property }
{
    switch (m_property.descriptor().valueDescriptor().type())
    {
        case dots::type::Type::boolean:
            m_color = ColorThemeActive.Keyword;
            break;
        case dots::type::Type::string:
            m_color = ColorThemeActive.StringType;
            m_valueQuoted = true;
            break;
        case dots::type::Type::Enum:
            m_color = ColorThemeActive.EnumType;
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
            m_color = ColorThemeActive.IntegralType;
            break;
    }

    update();
}

const dots::type::ProxyProperty<>& PropertyView::property() const
{
    return m_property;
}

bool PropertyView::less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyView& other) const
{
    auto compare = [&sortSpec](const auto& lhs, const auto& rhs)
    {
        if (sortSpec.SortDirection == ImGuiSortDirection_Ascending)
        {
            return std::less{}(lhs, rhs);
        }
        else
        {
            return std::greater{}(lhs, rhs);
        }
    };

    return compare(m_property, other.m_property);
}

bool PropertyView::isSelected() const
{
    return m_isSelected;
}

void PropertyView::update()
{
    if (m_valueQuoted)
    {
        m_value.clear();
        m_value += '"';
        m_value += dots::to_string(m_property);
        m_value += '"';
    }
    else
    {
        m_value = dots::to_string(m_property);
    }
}

void PropertyView::render()
{
    if (m_property.isValid())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, *m_color);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    }

    ImGui::Selectable(m_value.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
    ImGui::PopStyleColor();
}
