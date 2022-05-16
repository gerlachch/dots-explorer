#include <widgets/views/StructView.h>

StructView::StructView(const MetadataModel& metadataModel, const StructRefModel& structRefModel) :
    m_metadataModel{ metadataModel },
    m_structRefModel{ structRefModel }
{
    /* do nothing */
}

const MetadataModel& StructView::metadataModel() const
{
    return m_metadataModel;
}

const StructRefModel& StructView::structRefModel() const
{
    return m_structRefModel;
}

void StructView::render()
{
    // render header
    {
        ImGuiExt::TextColored(m_structRefModel.get().descriptorModel().declarationText());
    }

    ImGui::Separator();

    // render properties
    if (ImGui::BeginTable("PropertyTable", 2))
    {
        for (const PropertyModel& propertyModel : m_structRefModel.get().propertyModels())
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
        ImGuiExt::TextColored(m_metadataModel.get().lastOperationText());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Last Published:");
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(m_metadataModel.get().lastPublishedText());

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Last Published By:");
        ImGui::TableNextColumn();
        ImGuiExt::TextColored(m_metadataModel.get().lastPublishedByText());

        ImGui::EndTable();
    }
}
