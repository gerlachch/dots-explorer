#include <widgets/views/StructItem.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>
#include <DotsClient.dots.h>

StructItem::StructItem(EventModel model) :
    m_isSelected(false),
    m_isHovered(false),
    m_model(std::move(model))
{
    m_propertyModels.reserve(m_model.updatedInstanceModel().propertyModels().size());

    if (m_model.updatedInstanceModel().propertyModels().size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (const PropertyModel& propertyModel : m_model.updatedInstanceModel().propertyModels())
        {
            const dots::type::PropertyPath& propertyPath = propertyModel.descriptorModel().propertyPath();

            if (propertyPath.destination().valueDescriptor().type() == dots::type::Type::Struct)
            {
                continue;
            }

            m_propertyModels.emplace_back(propertyModel);
        }
    }
    else
    {
        for (const PropertyModel& propertyModel : m_model.updatedInstanceModel().propertyModels())
        {
            if (propertyModel.descriptorModel().propertyPath().elements().size() == 1)
            {
                m_propertyModels.emplace_back(propertyModel);
            }
        }
    }
}

const char* StructItem::widgetId() const
{
    if (m_widgetId.empty())
    {
        m_widgetId = fmt::format("StructItem-{}", M_nextWidgetId++);
    }

    return m_widgetId.data();
}

const EventModel& StructItem::model() const
{
    return m_model;
}

bool StructItem::less(const ImGuiTableSortSpecs& sortSpecs, const StructItem& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];
        ImS16 columnIndex = sortSpec.ColumnIndex;

        if (columnIndex >= MetaDataSize)
        {
            columnIndex -= MetaDataSize;
            const PropertyModel& propertyModelThis = m_propertyModels[columnIndex];
            const PropertyModel& propertyModelOther = other.m_propertyModels[columnIndex];

            if (propertyModelThis.less(sortSpec, propertyModelOther))
            {
                return true;
            }
            else if (propertyModelOther.less(sortSpec, propertyModelThis))
            {
                return false;
            }
        }
        else
        {
            auto compare = [&sortSpec](const auto& lhs, const auto& rhs)
            {
                if (sortSpec.SortDirection == ImGuiSortDirection_Ascending)
                {
                    return std::less{}(lhs.first, rhs.first);
                }
                else
                {
                    return std::greater{}(lhs.first, rhs.first);
                }
            };

            if (columnIndex == 0)
            {
                if (compare(m_model.metadataModel().lastOperationText(), other.m_model.metadataModel().lastOperationText()))
                {
                    return true;
                }
                else if (compare(other.m_model.metadataModel().lastOperationText(), m_model.metadataModel().lastOperationText()))
                {
                    return false;
                }
            }
            else if (columnIndex == 1)
            {
                if (compare(m_model.metadataModel().lastPublishedText(), other.m_model.metadataModel().lastPublishedText()))
                {
                    return true;
                }
                else if (compare(other.m_model.metadataModel().lastPublishedText(), m_model.metadataModel().lastPublishedText()))
                {
                    return false;
                }
            }
            else/* if (columnIndex == 2)*/
            {
                if (compare(m_model.metadataModel().lastPublishedByText(), other.m_model.metadataModel().lastPublishedByText()))
                {
                    return true;
                }
                else if (compare(other.m_model.metadataModel().lastPublishedByText(), m_model.metadataModel().lastPublishedByText()))
                {
                    return false;
                }
            }
        }
    }

    return &m_model.updatedInstanceModel().instance() < &other.m_model.updatedInstanceModel().instance();
}

bool StructItem::isSelected() const
{
    return m_isSelected;
}

bool StructItem::isHovered() const
{
    return m_isHovered;
}

void StructItem::render(bool hoverCondition)
{
    m_isHovered = false;

    // render meta data columns
    {
        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_model.metadataModel().lastOperationText());
            m_isHovered |= hoverCondition && ImGui::IsItemHovered();
        }

        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_model.metadataModel().lastPublishedText());
            m_isHovered |= hoverCondition && ImGui::IsItemHovered();
        }

        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_model.metadataModel().lastPublishedByText());
            m_isHovered |= hoverCondition && ImGui::IsItemHovered();
        }
    }

    // render property columns
    for (const PropertyModel& propertyModel : m_propertyModels)
    {
        if (ImGui::TableNextColumn())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, propertyModel.valueText().second);
            ImGui::Selectable(propertyModel.valueText().first.data(), &m_isSelected, ImGuiSelectableFlags_SpanAllColumns);
            ImGui::PopStyleColor();
            m_isHovered |= hoverCondition && ImGui::IsItemHovered();
        }
    }
}

void StructItem::renderTooltip() const
{
    ImGui::BeginTooltip();

    // render header
    {
        ImGuiExt::TextColored(m_model.updatedInstanceModel().descriptorModel().declarationText());
    }

    ImGui::Separator();

    // render properties
    if (ImGui::BeginTable("PropertyTable", 2))
    {
        for (const PropertyModel& propertyModel : m_model.updatedInstanceModel().propertyModels())
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGuiExt::TextColored(propertyModel.descriptorModel().declarationText());

            ImGui::TableNextColumn();

            if (propertyModel.property().descriptor().valueDescriptor().type() != dots::type::Type::Struct)
            {
                ImGuiExt::TextColored(propertyModel.valueText());
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    // render meta data
    if (ImGui::BeginTable("MetaDataTable", 2))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Last Operation:");
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(m_model.metadataModel().lastOperationText());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Last Published:");
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(m_model.metadataModel().lastPublishedText());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Last Published By:");
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(m_model.metadataModel().lastPublishedByText());

        ImGui::EndTable();
    }

    ImGui::EndTooltip();
}
