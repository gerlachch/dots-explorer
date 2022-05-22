#include <widgets/views/StructItem.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>
#include <DotsClient.dots.h>

StructItem::StructItem(const StructDescriptorModel& structDescriptorModel, const PublisherModel& publisherModel, const dots::type::Struct& instance) :
    m_isSelected(false),
    m_isHovered(false),
    m_metadataModel{ publisherModel },
    m_structRefModel{ structDescriptorModel, instance }
{
    m_propertyModels.reserve(m_structRefModel.propertyModels().size());

    if (m_structRefModel.propertyModels().size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (PropertyModel& propertyModel : m_structRefModel.propertyModels())
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
        for (PropertyModel& propertyModel : m_structRefModel.propertyModels())
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

const MetadataModel& StructItem::metadataModel() const
{
    return m_metadataModel;
}

MetadataModel& StructItem::metadataModel()
{
    return m_metadataModel;
}

const StructRefModel& StructItem::structRefModel() const
{
    return m_structRefModel;
}

StructRefModel& StructItem::structRefModel()
{
    return m_structRefModel;
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
                if (compare(m_metadataModel.lastOperationText(), other.m_metadataModel.lastOperationText()))
                {
                    return true;
                }
                else if (compare(other.m_metadataModel.lastOperationText(), m_metadataModel.lastOperationText()))
                {
                    return false;
                }
            }
            else if (columnIndex == 1)
            {
                if (compare(m_metadataModel.lastPublishedText(), other.m_metadataModel.lastPublishedText()))
                {
                    return true;
                }
                else if (compare(other.m_metadataModel.lastPublishedText(), m_metadataModel.lastPublishedText()))
                {
                    return false;
                }
            }
            else/* if (columnIndex == 2)*/
            {
                if (compare(m_metadataModel.lastPublishedByText(), other.m_metadataModel.lastPublishedByText()))
                {
                    return true;
                }
                else if (compare(other.m_metadataModel.lastPublishedByText(), m_metadataModel.lastPublishedByText()))
                {
                    return false;
                }
            }
        }
    }

    return &structRefModel().instance() < &other.structRefModel().instance();
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
            ImGuiExt::TextColored(m_metadataModel.lastOperationText());
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
