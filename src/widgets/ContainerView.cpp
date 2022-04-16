#include <widgets/ContainerView.h>
#include <imgui.h>

ContainerView::ContainerView(const dots::type::StructDescriptor<>& descriptor) :
    m_descriptor{ descriptor }
{
    /* do nothing */
}

void ContainerView::render()
{
    constexpr ImGuiTableFlags TableFlags = 
        ImGuiTableFlags_Borders        |
        ImGuiTableFlags_BordersH       |
        ImGuiTableFlags_BordersOuterH  |
        ImGuiTableFlags_BordersInnerH  |
        ImGuiTableFlags_BordersV       |
        ImGuiTableFlags_BordersOuterV  |
        ImGuiTableFlags_BordersInnerV  |
        ImGuiTableFlags_BordersOuter   |
        ImGuiTableFlags_BordersInner   |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_Resizable
    ;

    const dots::type::StructDescriptor<>& descriptor = m_descriptor;

    if (ImGui::BeginTable(descriptor.name().data(), static_cast<int>(descriptor.propertyDescriptors().size()), TableFlags))
    {
        for (const dots::type::PropertyDescriptor& propertyDescriptor : descriptor.propertyDescriptors())
        {
            ImGui::TableSetupColumn(propertyDescriptor.name().data());
        }

        ImGui::TableHeadersRow();

        for (const auto& [instance, cloneInfo] : dots::container(descriptor))
        {
            for (auto property : instance)
            {
                ImGui::TableNextColumn();

                if (property.isValid())
                {
                    std::string value = dots::to_string(property);
                    
                    switch (property.descriptor().valueDescriptor().type())
                    {
                        case dots::type::Type::boolean:
                            ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::string:
                            ImGui::TextColored(ImVec4{ 0.91f, 0.79f, 0.73f, 1.0f }, "\"%s\"", value.data());
                            break;
                        case dots::type::Type::int8:
                        case dots::type::Type::uint8:
                        case dots::type::Type::int16:
                        case dots::type::Type::uint16:
                        case dots::type::Type::int32:
                        case dots::type::Type::uint32:
                        case dots::type::Type::int64:
                        case dots::type::Type::uint64:
                        case dots::type::Type::float32:
                        case dots::type::Type::float64:
                            ImGui::TextColored(ImVec4{ 0.72f, 0.84f, 0.64f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::Enum:
                            ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::property_set:
                        case dots::type::Type::timepoint:
                        case dots::type::Type::steady_timepoint:
                        case dots::type::Type::duration:
                        case dots::type::Type::uuid:
                        case dots::type::Type::Vector:
                        case dots::type::Type::Struct:
                        default: 
                            ImGui::TextUnformatted(value.data());
                    }
                }
                else
                {
                    ImGui::TextDisabled("<invalid>");
                }
            }
        }

        ImGui::EndTable();
    }
}
