#include <widgets/views/TraceItem.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>

TraceItem::TraceItem(size_t index, const StructDescriptorModel& structDescriptorModel, const PublisherModel& publisherModel, const dots::Event<>& event) :
    m_isSelected(false),
    m_isHovered(false),
    m_index(index),
    m_indexText{ fmt::format("#{}", m_index), ColorThemeActive.Disabled },
    m_publishedInstance(event.transmitted()),
    m_metadataModel{ publisherModel },
    m_structRefModel{ structDescriptorModel, *m_publishedInstance }
{
    m_metadataModel.fetch(event);
}

const char* TraceItem::widgetId() const
{
    if (m_widgetId.empty())
    {
        m_widgetId = fmt::format("TraceItem-{}", M_nextWidgetId++);
    }

    return m_widgetId.data();
}

size_t TraceItem::index() const
{
    return m_index;
}

const StructRefModel& TraceItem::structRefModel() const
{
    return m_structRefModel;
}

const MetadataModel& TraceItem::metadataModel() const
{
    return m_metadataModel;
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
        const auto& [text, color] = m_indexText;
        ImGui::PushStyleColor(ImGuiCol_Header, ColorThemeActive.Marker);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Selectable(text.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
        ImGui::PopStyleColor(2);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_metadataModel.lastPublishedText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_metadataModel.lastPublishedByText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_metadataModel.lastOperationText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_structRefModel.descriptorModel().declarationText()[1]);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(m_structRefModel.descriptorModel().declarationText()[1]);

        for (const PropertyModel& propertyModel : m_structRefModel.propertyModels())
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
