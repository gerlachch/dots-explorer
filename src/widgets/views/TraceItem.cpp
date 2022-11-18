#include <widgets/views/TraceItem.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>

TraceItem::TraceItem(std::shared_ptr<const EventModel> model) :
    m_isSelected(false),
    m_isHovered(false),
    m_model(std::move(model))
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

const EventModel& TraceItem::model() const
{
    return *m_model;
}

const std::shared_ptr<const EventModel>& TraceItem::modelPtr() const
{
    return m_model;
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
        m_filterText += fmt::format(" {}", model().updatedInstanceModel().descriptorModel().declarationText()[1].first);
    }

    if (*targets.publishedAt)
    {
        m_filterText += fmt::format(" {}", model().metadataModel().lastPublishedText().first);
    }

    if (*targets.publishedBy)
    {
        m_filterText += fmt::format(" {}", model().metadataModel().lastPublishedByText().first);
    }

    if (*targets.operation)
    {
        m_filterText += fmt::format(" {}", model().metadataModel().lastOperationText().first);
    }

    if (*targets.instance)
    {
        for (const PropertyModel& propertyModel : model().updatedInstanceModel().propertyModels())
        {
            if (propertyModel.property().isValid())
            {
                m_filterText += fmt::format(" {}:{}", propertyModel.descriptorModel().propertyPath().destination().name(), propertyModel.valueText().first);
            }
        }
    }

    m_filterTextLower.clear();
    std::transform(m_filterText.begin(), m_filterText.end(), std::back_inserter(m_filterTextLower), [](unsigned char c){ return std::tolower(c); });
}

bool TraceItem::isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const
{
    const dots::type::StructDescriptor& descriptor = model().updatedInstanceModel().descriptorModel().descriptor();

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
            return filter->match(*filterSettings.activeFilter->matchCase ? m_filterText : m_filterTextLower);
        }
    }
}

void TraceItem::render(bool hoverCondition)
{
    m_isHovered = false;

    if (ImGui::TableNextColumn())
    {
        const auto& [text, color] = model().indexText();
        ImGui::PushStyleColor(ImGuiCol_Header, ColorThemeActive.Marker);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Selectable(text.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
        ImGui::PopStyleColor(2);
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(model().metadataModel().lastPublishedText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(model().metadataModel().lastPublishedByText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    if (ImGui::TableNextColumn())
    {
        ImGuiExt::TextColored(model().metadataModel().lastOperationText());
        m_isHovered |= hoverCondition && ImGui::IsItemHovered();
    }

    auto render_instance = [this, &hoverCondition](const StructModel& structModel)
    {
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
    };

    if (ImGui::TableNextColumn())
    {
        render_instance(model().publishedInstanceModel());
    }

    if (ImGui::TableNextColumn())
    {
        render_instance(model().updatedInstanceModel());
    }
}
