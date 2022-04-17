#include <widgets/PoolView.h>
#include <imgui.h>
#include <widgets/ContainerView.h>
#include <DotsDescriptorRequest.dots.h>

PoolView::PoolView() :
    m_poolChanged(false),
    m_subscription{ dots::subscribe<dots::type::StructDescriptor<>>({ &PoolView::update, this }) }
{
    /* do nothing */
}

void PoolView::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (descriptor.cached() && !descriptor.substructOnly() && !descriptor.internal())
    {
        m_containerViews.emplace_back(std::make_unique<ContainerView>(descriptor));
        m_poolChanged = true;
    }
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
        ImGuiTableFlags_ScrollY       |
        ImGuiTableFlags_Sortable
    ;

    if (ImGui::BeginTable("Cached Types", 2, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // create headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // sort container views
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_poolChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_containerViews.begin(), m_containerViews.end(), [sortSpecs](const auto& lhs, const auto& rhs)
            {
                return lhs->less(*sortSpecs, *rhs);
            });

            m_poolChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render container views
        for (auto& containerView : m_containerViews) 
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool containerOpen = ImGui::TreeNodeEx(containerView->container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);

            ImGui::TableNextColumn();
            ImGui::Text("%zu", containerView->container().size());

            if (containerOpen)
            {
                ImGui::TableNextColumn();
                containerView->render();

                ImGui::TreePop();
            }
        }

        ImGui::EndTable();
    }
}
