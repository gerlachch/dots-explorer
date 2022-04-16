#include <widgets/PoolView.h>
#include <imgui.h>
#include <widgets/ContainerView.h>
#include <DotsDescriptorRequest.dots.h>

PoolView::PoolView() :
    m_poolChanged(false),
    m_subscription{ dots::subscribe<dots::type::StructDescriptor<>>([this](const dots::type::StructDescriptor<>& descriptor)
    {
        if (descriptor.cached() && !descriptor.substructOnly() && !descriptor.internal())
        {
            m_containerViews.emplace_back(descriptor);
            m_poolChanged = true;
        }
    }) }
{
    /* do nothing */
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
        ImGui::TableSetupColumn("No. Instances", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // sort container views
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_poolChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_containerViews.begin(), m_containerViews.end(), [sortSpecs](const ContainerView& lhs, const ContainerView& rhs)
            {
                for (int i = 0; i < sortSpecs->SpecsCount; ++i)
                {
                    const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs->Specs[i];

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

                    bool less = false;

                    switch (sortSpec.ColumnIndex)
                    {
                        case 0:  less = compare(lhs.container().descriptor().name(), rhs.container().descriptor().name()); break;
                        case 1:  less = compare(lhs.container().size(), rhs.container().size()); break;
                        default: IM_ASSERT(0); break;
                    }

                    if (less)
                    {
                        return true;
                    }
                }

                return false;
            });

            m_poolChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render container views
        for (ContainerView& containerView : m_containerViews) 
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool containerOpen = ImGui::TreeNodeEx(containerView.container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);

            ImGui::TableNextColumn();
            ImGui::Text("%zu", containerView.container().size());

            if (containerOpen)
            {
                ImGui::TableNextColumn();
                containerView.render();

                ImGui::TreePop();
            }
        }

        ImGui::EndTable();
    }
}
