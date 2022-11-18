#include <widgets/views/EventView.h>
#include <fmt/format.h>

EventView::EventView(std::shared_ptr<const EventModel> model) :
    m_model(std::move(model))
{
    /* do nothing */
}

void EventView::render() const
{
    // render header
    {
        ImGuiExt::TextColored(m_model->publishedInstanceModel().descriptorModel().declarationText());
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

                    bool highlight = propertyDescriptorModel.propertyPath().elements().front().get().set() <= m_model->metadataModel().lastPublishedProperties();
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

        render_instance(m_model->publishedInstanceModel());
        render_instance(m_model->updatedInstanceModel());

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

        render_metadata("Last Operation:", m_model->metadataModel().lastOperationText());
        render_metadata("Last Published:", m_model->metadataModel().lastPublishedText());
        render_metadata("Last Published By:", m_model->metadataModel().lastPublishedByText());

        ImGui::EndTable();
    }
}
