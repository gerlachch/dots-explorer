#include <widgets/PoolView.h>
#include <imgui.h>
#include <widgets/ContainerView.h>
#include <DotsDescriptorRequest.dots.h>

PoolView::PoolView()
{
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor<>>([this](const dots::type::StructDescriptor<>& descriptor)
    {
        if (descriptor.cached() && !descriptor.substructOnly() && !descriptor.internal())
        {
            m_subscriptions.emplace_back(dots::subscribe(descriptor, [](const dots::Event<>&/* event*/){}));
        }
    }));
}

void PoolView::render()
{
    constexpr ImGuiTableFlags TableFlags = 
        ImGuiTableFlags_Borders       |
        ImGuiTableFlags_BordersH      |
        ImGuiTableFlags_BordersOuterH |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_BordersV      |
        ImGuiTableFlags_BordersOuterV |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersOuter  |
        ImGuiTableFlags_BordersInner  |
        ImGuiTableFlags_ScrollY
    ;

    if (ImGui::BeginTable("Cached Types", 2, TableFlags, ImGui::GetContentRegionAvail()))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("No. Instances", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto& [descriptor, container] : dots::pool()) 
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool containerOpen = ImGui::TreeNodeEx(descriptor->name().data(), ImGuiTreeNodeFlags_SpanFullWidth);

            ImGui::TableNextColumn();
            ImGui::Text("%zu", container.size());

            if (containerOpen)
            {
                ImGui::TableNextColumn();
                ContainerView containerView{ *descriptor };
                containerView.render();

                ImGui::TreePop();
            }
        }

        ImGui::EndTable();
    }
}
