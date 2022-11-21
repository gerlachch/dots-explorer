#include <widgets/views/TraceItem.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>

TraceItem::TraceItem(EventModel model) :
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
        m_filterText += fmt::format(" {}", model().descriptorModel().declarationText()[1].first);
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
    const dots::type::StructDescriptor& descriptor = model().descriptorModel().descriptor();

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

void TraceItem::renderTooltip() const
{
    ImGui::BeginTooltip();

    // render header
    {
        ImGuiExt::TextColored(m_model.descriptorModel().declarationText());
    }

    ImGui::Separator();

    // render instances
    if (ImGui::BeginTable("EventTraceQuickInfo", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Published Instance");
        ImGui::TableSetupColumn("Updated Instance");
        ImGui::TableHeadersRow();

        auto render_instance = [this](const StructModel& instanceModel)
        {
            ImGui::TableNextColumn();

            if (ImGui::BeginTable(fmt::format("PropertyTable-{}", reinterpret_cast<const void*>(&instanceModel)).data(), 2))
            {
                for (const PropertyModel& propertyModel : instanceModel.propertyModels())
                {
                    const PropertyDescriptorModel& propertyDescriptorModel = propertyModel.descriptorModel();

                    bool highlight = propertyDescriptorModel.propertyPath().elements().front().get().set() <= m_model.metadataModel().lastPublishedProperties();
                    ImGui::BeginDisabled(!highlight);

                    ImGui::TableNextColumn();
                    ImGuiExt::TextColored(propertyDescriptorModel.declarationText());

                    ImGui::TableNextColumn();
                    if (propertyModel.property().descriptor().valueDescriptor().type() != dots::type::Type::Struct)
                    {
                        ImGuiExt::TextColored(propertyModel.valueText());
                    }

                    ImGui::EndDisabled();
                }

                ImGui::EndTable();
            }
        };

        render_instance(m_model.publishedInstanceModel());
        render_instance(m_model.updatedInstanceModel());

        ImGui::EndTable();
    }

    ImGui::Separator();

    // render meta data
    if (ImGui::BeginTable("MetaDataTable", 2))
    {
        auto render_metadata = [](std::string_view metadataName, const ImGuiExt::ColoredText& text)
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(metadataName.data(), metadataName.data() + metadataName.size());
            ImGui::TableNextColumn();
            ImGuiExt::TextColored(text);
        };

        render_metadata("Last Operation:", m_model.metadataModel().lastOperationText());
        render_metadata("Last Published:", m_model.metadataModel().lastPublishedText());
        render_metadata("Last Published By:", m_model.metadataModel().lastPublishedByText());

        ImGui::EndTable();
    }

    ImGui::EndTooltip();
}
