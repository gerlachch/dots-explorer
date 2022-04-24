#include <widgets/PropertyView.h>
#include <common/Colors.h>

PropertyView::PropertyView(const dots::type::ProxyProperty<>& property) :
    m_isSelected(false),
    m_property{ property }
{
    /* do nothing */
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
    m_value.clear();
}

void PropertyView::render(const PropertyDescription& propertyDescription)
{
    if (m_value.empty())
    {
        if (propertyDescription.valueQuoted())
        {
            m_value += '"';
            m_value += dots::to_string(m_property);
            m_value += '"';
        }
        else
        {
            m_value = dots::to_string(m_property);
        }
    }

    if (m_property.isValid())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, propertyDescription.valueColor());
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ColorThemeActive.Disabled);
    }

    ImGui::Selectable(m_value.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
    ImGui::PopStyleColor();
}
