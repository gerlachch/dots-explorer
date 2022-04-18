#include <widgets/PropertyView.h>

PropertyView::PropertyView(const dots::type::ProxyProperty<>& property) :
    m_valueQuoted(false),
    m_property{ property }
{
    switch (m_property.descriptor().valueDescriptor().type())
    {
        case dots::type::Type::boolean:
            m_color = ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f };
            break;
        case dots::type::Type::string:
            m_color = ImVec4{0.91f, 0.79f, 0.73f, 1.0f };
            m_valueQuoted = true;
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
            m_color = ImVec4{ 0.72f, 0.84f, 0.64f, 1.0f };
            break;
        case dots::type::Type::Enum:
            m_color = ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f };
            break;
        case dots::type::Type::property_set:
        case dots::type::Type::timepoint:
        case dots::type::Type::steady_timepoint:
        case dots::type::Type::duration:
        case dots::type::Type::uuid:
        case dots::type::Type::Vector:
        case dots::type::Type::Struct:
        default:
            m_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
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

    ImGui::TextUnformatted(m_value.data());
    ImGui::PopStyleColor();
}
