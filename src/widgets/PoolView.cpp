#include <widgets/PoolView.h>
#include <string_view>
#include <regex>
#include <imgui.h>
#include <widgets/ContainerView.h>
#include <DotsDescriptorRequest.dots.h>

PoolView::PoolView() :
    m_containerFilter{},
    m_poolChanged(false),
    m_subscription{ dots::subscribe<dots::type::StructDescriptor<>>({ &PoolView::update, this }) }
{
    /* do nothing */
}

void PoolView::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (descriptor.cached() && !descriptor.substructOnly() && !descriptor.internal())
    {
        m_containerViews.emplace_back(std::make_shared<ContainerView>(descriptor));
        m_poolChanged = true;
    }
}

void PoolView::render()
{
    // control area
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Filter");

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.2f);
        if (ImGui::InputTextWithHint("##containerFilter", "<none>", m_containerFilter.data(), m_containerFilter.size()) || m_poolChanged)
        {
            if (std::string_view containerFilter = m_containerFilter.data(); containerFilter.empty())
            {
                m_containerViewsFiltered = m_containerViews;
            }
            else
            {
                m_containerViewsFiltered.clear();
                std::regex regex{ containerFilter.data() };
                std::copy_if(m_containerViews.begin(), m_containerViews.end(), std::back_inserter(m_containerViewsFiltered), [&regex](const auto& containerView)
                {
                    return std::regex_search(containerView->container().descriptor().name(), regex);
                });
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Types can be filtered by specifying substrings or ECMAScript regular expressions.");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // container views
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

    const dots::type::StructDescriptor<>* instanceEditDescriptor = nullptr;
    
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
            std::sort(m_containerViewsFiltered.begin(), m_containerViewsFiltered.end(), [sortSpecs](const auto& lhs, const auto& rhs)
            {
                return lhs->less(*sortSpecs, *rhs);
            });

            m_poolChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render container views
        for (auto& containerView : m_containerViewsFiltered) 
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool containerOpen = ImGui::TreeNodeEx(containerView->container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);

            // context menu
            {
                if (ImGui::BeginPopupContextItem())
                {
                    ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, "struct");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, "%s", containerView->container().descriptor().name().data());
                    ImGui::Separator();

                    if (ImGui::MenuItem("Create/Update"))
                    {
                        instanceEditDescriptor = &containerView.get()->container().descriptor();
                    }

                    ImGui::EndPopup();
                }
            }

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

    // instance edit
    {
        if (instanceEditDescriptor != nullptr)
        {
            m_instanceEdit.emplace(*instanceEditDescriptor);
        }

        if (m_instanceEdit != std::nullopt && !m_instanceEdit->render())
        {
            m_instanceEdit = std::nullopt;
        }
    }
}
