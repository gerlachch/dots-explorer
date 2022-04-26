#include <widgets/PropertyView.h>
#include <common/Colors.h>

PropertyView::PropertyView(PropertyModel& model) :
    m_isSelected(false),
    m_model{ model }
{
    /* do nothing */
}

const PropertyModel& PropertyView::model() const
{
    return m_model;
}

PropertyModel& PropertyView::model()
{
    return m_model;
}

bool PropertyView::isSelected() const
{
    return m_isSelected;
}

void PropertyView::render()
{
    PropertyModel& model = m_model.get();
    ImGui::PushStyleColor(ImGuiCol_Text, model.valueText().second);
    ImGui::Selectable(model.valueText().first.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
    ImGui::PopStyleColor();
}
