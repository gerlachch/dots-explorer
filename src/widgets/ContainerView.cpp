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
                std::string value = dots::to_string(property);
                ImGui::TextUnformatted(value.data());
            }
        }

        ImGui::EndTable();
    }
}
