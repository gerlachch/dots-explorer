#include <widgets/InstanceView.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <common/ImGuiExt.h>
#include <DotsClient.dots.h>

InstanceView::InstanceView(const StructDescriptorModel& structDescriptorModel, const dots::type::Struct& instance) :
    m_widgetId{ fmt::format("InstanceView-{}", M_nextWidgetId++) },
    m_structModel{ structDescriptorModel, instance }
{
    if (m_structModel.propertyModels().size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (PropertyModel& propertyModel : m_structModel.propertyModels())
        {
            const dots::type::PropertyPath& propertyPath = propertyModel.descriptorModel().propertyPath();

            if (propertyPath.destination().valueDescriptor().type() == dots::type::Type::Struct)
            {
                continue;
            }

            m_propertyViews.emplace_back(propertyModel);
        }
    }
    else
    {
        for (PropertyModel& propertyModel : m_structModel.propertyModels())
        {
            if (propertyModel.descriptorModel().propertyPath().elements().size() == 1)
            {
                m_propertyViews.emplace_back(propertyModel);
            }
        }
    }
}

const char* InstanceView::widgetId() const
{
    return m_widgetId.data();
}

const MetadataModel& InstanceView::metadataModel() const
{
    return m_metadataModel;
}

MetadataModel& InstanceView::metadataModel()
{
    return m_metadataModel;
}

const StructModel& InstanceView::structModel() const
{
    return m_structModel;
}

StructModel& InstanceView::structModel()
{
    return m_structModel;
}

bool InstanceView::less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];
        ImS16 columnIndex = sortSpec.ColumnIndex;

        if (columnIndex >= MetaDataSize)
        {
            columnIndex -= MetaDataSize;
            const PropertyView& propertyViewThis = m_propertyViews[columnIndex];
            const PropertyView& propertyViewOther = other.m_propertyViews[columnIndex];

            if (propertyViewThis.model().less(sortSpec, propertyViewOther.model()))
            {
                return true;
            }
            else if (propertyViewOther.model().less(sortSpec, propertyViewThis.model()))
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

    return structModel().instance()._less(other.structModel().instance(), structModel().instance()._keyProperties());
}

bool InstanceView::isSelected() const
{
    return std::any_of(m_propertyViews.begin(), m_propertyViews.end(), [](const PropertyView& propertyView){ return propertyView.isSelected(); });
}

void InstanceView::render()
{
    // render meta data columns
    {
        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_metadataModel.lastOperationText());
        }

        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_metadataModel.lastPublishedText());
        }

        if (ImGui::TableNextColumn())
        {
            ImGuiExt::TextColored(m_metadataModel.lastPublishedByText());
        }
    }

    // render property columns
    for (PropertyView& propertyView : m_propertyViews)
    {
        ImGui::TableNextColumn();
        propertyView.render();
    }

    // render quick info tooltip
    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
    {
        ImGui::BeginTooltip();

        // render header
        {
            ImGuiExt::TextColored(m_structModel.descriptorModel().declarationText());
        }

        ImGui::Separator();

        // render properties
        if (ImGui::BeginTable("PropertyTable", 2))
        {
            for (const PropertyModel& propertyModel : m_structModel.propertyModels())
            {
                ImGui::TableNextRow();

                if (m_structModel.propertyModels().size() <= IMGUI_TABLE_MAX_COLUMNS)
                {
                    ImGui::TableNextColumn();
                    ImGuiExt::TextColored(propertyModel.descriptorModel().declarationText());

                    ImGui::TableNextColumn();

                    if (propertyModel.property().descriptor().valueDescriptor().type() != dots::type::Type::Struct)
                    {
                        ImGuiExt::TextColored(propertyModel.valueText());
                    }
                }
                else
                {
                    ImGui::TableNextColumn();
                    ImGuiExt::TextColored(propertyModel.descriptorModel().declarationText());

                    ImGui::TableNextColumn();
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
            ImGuiExt::TextColored(m_metadataModel.lastOperationText());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Last Published:");
            ImGui::TableNextColumn();
            ImGuiExt::TextColored(m_metadataModel.lastPublishedText());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Last Published By:");
            ImGui::TableNextColumn();
            ImGuiExt::TextColored(m_metadataModel.lastPublishedByText());

            ImGui::EndTable();
        }

        ImGui::EndTooltip();
    }
}
