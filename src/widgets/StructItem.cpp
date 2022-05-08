#include <widgets/StructItem.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>
#include <DotsClient.dots.h>

StructItem::StructItem(const StructDescriptorModel& structDescriptorModel, const dots::type::Struct& instance) :
    m_isSelected(false),
    m_isHovered(false),
    m_structModel{ structDescriptorModel, instance }
{
    m_propertyModels.reserve(m_structModel.propertyModels().size());

    if (m_structModel.propertyModels().size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (PropertyModel& propertyModel : m_structModel.propertyModels())
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
        for (PropertyModel& propertyModel : m_structModel.propertyModels())
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

const StructModel& StructItem::structModel() const
{
    return m_structModel;
}

StructModel& StructItem::structModel()
{
    return m_structModel;
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
                    return std::less{}(lhs, rhs);
                }
                else
                {
                    return std::greater{}(lhs, rhs);
                }
            };

            const std::string& metaDataStrThis = m_metadataModel.metadataText()[columnIndex].first;
            const std::string& metaDataStrOther = other.m_metadataModel.metadataText()[columnIndex].first;

            if (compare(metaDataStrThis, metaDataStrOther))
            {
                return true;
            }
            else if (compare(metaDataStrOther, metaDataStrThis))
            {
                return false;
            }
        }
    }

    return &structModel().instance() < &other.structModel().instance();
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
