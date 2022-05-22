#include <widgets/views/EventView.h>
#include <fmt/format.h>

EventView::EventView(const MetadataModel& metadataModel, const StructRefModel& publishedInstanceModel, const StructRefModel& updatedInstanceModel) :
    m_metadataModel{ metadataModel },
    m_publishedInstanceModel{ publishedInstanceModel },
    m_updatedInstanceModel{ updatedInstanceModel }
{
    /* do nothing */
}

void EventView::render() const
{
    // render header
    {
        ImGuiExt::TextColored(m_publishedInstanceModel.get().descriptorModel().declarationText());
    }

    ImGui::Separator();

    // render instances
    if (ImGui::BeginTable("EventTraceQuickInfo", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Published Instance");
        ImGui::TableSetupColumn("Updated Instance");
        ImGui::TableHeadersRow();

        auto render_instance = [this](const StructRefModel& instanceModel)
        {
            ImGui::TableNextColumn();

            if (ImGui::BeginTable(fmt::format("PropertyTable-{}", reinterpret_cast<const void*>(&instanceModel)).data(), 2))
            {
                for (const PropertyModel& propertyModel : instanceModel.propertyModels())
                {
                    const PropertyDescriptorModel& propertyDescriptorModel = propertyModel.descriptorModel();

                    bool highlight = propertyDescriptorModel.propertyPath().elements().front().get().set() <= m_metadataModel.get().lastPublishedProperties();
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

        render_instance(m_publishedInstanceModel);
        render_instance(m_updatedInstanceModel);

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

        render_metadata("Last Operation:", m_metadataModel.get().lastOperationText());
        render_metadata("Last Published:", m_metadataModel.get().lastPublishedText());
        render_metadata("Last Published By:", m_metadataModel.get().lastPublishedByText());

        ImGui::EndTable();
    }
}
