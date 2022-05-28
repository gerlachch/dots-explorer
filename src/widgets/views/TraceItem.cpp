#include <widgets/views/TraceItem.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>
#include <dots_ext/struct_ops.h>

TraceItem::TraceItem(size_t index, const StructDescriptorModel& structDescriptorModel, const PublisherModel& publisherModel, const dots::Event<>& event) :
    m_isSelected(false),
    m_isHovered(false),
    m_index(index),
    m_indexText{ fmt::format("#{}", m_index), ColorThemeActive.Disabled },
    m_publishedInstance{ copy_all(event.transmitted()) },
    m_updatedInstance{ copy_all(event.updated()) },
    m_metadataModel{ publisherModel },
    m_publishedInstanceModel{ structDescriptorModel, *m_publishedInstance },
    m_updatedInstanceModel{ structDescriptorModel, *m_updatedInstance }
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

const MetadataModel& TraceItem::metadataModel() const
{
    return m_metadataModel;
}
const StructRefModel& TraceItem::publishedInstanceModel() const
{
    return m_publishedInstanceModel;
}

const StructRefModel& TraceItem::updatedInstanceModel() const
{
    return m_updatedInstanceModel;
}

bool TraceItem::isSelected() const
{
    return m_isSelected;
}

bool TraceItem::isHovered() const
{
    return m_isHovered;
}

void TraceItem::setFilterTargets(const FilterTargets& targets)
{
    m_filterText.clear();

    if (*targets.type)
    {
        m_filterText += fmt::format(" {}", m_publishedInstanceModel.descriptorModel().declarationText()[1].first);
    }

    if (*targets.publisher)
    {
        m_filterText += fmt::format(" {}", m_metadataModel.lastPublishedByText().first);
    }

    if (*targets.instance)
    {
        for (const PropertyModel& propertyModel : m_updatedInstanceModel.propertyModels())
        {
            if (propertyModel.property().isValid())
            {
                m_filterText += fmt::format(" {}:{}", propertyModel.descriptorModel().propertyPath().destination().name(), propertyModel.valueText().first);
            }
        }
    }

    m_filterTextLower.clear();
    std::transform(m_filterText.begin(), m_filterText.end(), std::back_inserter(m_filterTextLower), std::tolower);
}

bool TraceItem::isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const
{
    const dots::type::StructDescriptor<>& descriptor = publishedInstanceModel().descriptorModel().descriptor();

    if (descriptor.internal() && !*filterSettings.types->internal)
    {
        return false;
    }
    else if (!descriptor.cached() && !*filterSettings.types->uncached)
    {
        return false;
    }
    else
    {
        if (filterSettings.activeFilter->expression->empty())
        {
            return true;
        }
        else if (filter == std::nullopt)
        {
            return false;
        }
        else
        {
            return filter->match(filterSettings.activeFilter->matchCase ? m_filterText : m_filterTextLower);
        }
    }
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
        ImGuiExt::TextColored(m_publishedInstanceModel.descriptorModel().declarationText()[1]);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    auto render_instance = [this, &hoverCondition](const StructRefModel& structRefModel)
    {
        ImGuiExt::TextColored(structRefModel.descriptorModel().declarationText()[1]);

        for (const PropertyModel& propertyModel : structRefModel.propertyModels())
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
    };

    if (ImGui::TableNextColumn())
    {
        render_instance(m_publishedInstanceModel);
    }

    if (ImGui::TableNextColumn())
    {
        render_instance(m_updatedInstanceModel);
    }
}
