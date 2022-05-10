#include <widgets/TraceItem.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>

TraceItem::TraceItem(size_t index, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event) :
    m_isSelected(false),
    m_isHovered(false),
    m_eventModel{ index, structDescriptorModel, event }
{
    /* do nothing */
}

const char* TraceItem::widgetId() const
{
    if (m_widgetId.empty())
    {
        m_widgetId = fmt::format("TraceItem-{}", M_nextWidgetId++);
    }

    return m_widgetId.data();
}

const EventModel& TraceItem::eventModel() const
{
    return m_eventModel;
}

EventModel& TraceItem::eventModel()
{
    return m_eventModel;
}

bool TraceItem::isSelected() const
{
    return m_isSelected;
}

bool TraceItem::isHovered() const
{
    return m_isHovered;
}

void TraceItem::render(bool hoverCondition)
{
    m_isHovered = false;

    if (ImGui::TableNextColumn())
    {
        const auto& [text, color] = m_eventModel.indexText();
        ImGui::PushStyleColor(ImGuiCol_Header, ColorThemeActive.Marker);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Selectable(text.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
        ImGui::PopStyleColor(2);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_eventModel.metadataModel().lastPublishedText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_eventModel.metadataModel().lastPublishedByText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_eventModel.metadataModel().lastOperationText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_eventModel.structModel().descriptorModel().declarationText()[1]);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        const StructModel& structModel = m_eventModel.structModel();
        ImGuiExt::TextColored(structModel.descriptorModel().declarationText()[1]);

        for (const PropertyModel& propertyModel : structModel.propertyModels())
        {
            if (!propertyModel.property().isValid())
            {
                continue;
            }

            ImGui::SameLine(0, 0);
            ImGui::TextColored(ColorThemeActive.Identifier, " %s:", propertyModel.descriptorModel().propertyPath().destination().name().data());

            ImGui::SameLine(0, 0);
            ImGuiExt::TextColored(propertyModel.valueText());
        }

        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }
}
