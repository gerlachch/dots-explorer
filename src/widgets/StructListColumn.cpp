#include <widgets/StructListColumn.h>
#include <common/Colors.h>

StructListColumn::StructListColumn(PropertyModel& model) :
    m_isSelected(false),
    m_model{ model }
{
    /* do nothing */
}

const PropertyModel& StructListColumn::model() const
{
    return m_model;
}

PropertyModel& StructListColumn::model()
{
    return m_model;
}

bool StructListColumn::isSelected() const
{
    return m_isSelected;
}

void StructListColumn::render()
{
    PropertyModel& model = m_model.get();
    ImGui::PushStyleColor(ImGuiCol_Text, model.valueText().second);
    ImGui::Selectable(model.valueText().first.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
    ImGui::PopStyleColor();
}
