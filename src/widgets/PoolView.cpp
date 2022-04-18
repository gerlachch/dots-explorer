#include <widgets/PoolView.h>
#include <string_view>
#include <regex>
#include <imgui.h>
#include <widgets/ContainerView.h>

PoolView::PoolView() :
    m_containerFilterBuffer(256, '\0'),
    m_poolChanged(false),
    m_showInternal(false),
    m_subscription{ dots::subscribe<dots::type::StructDescriptor<>>({ &PoolView::update, this }) }
{
    /* do nothing */
}

void PoolView::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (descriptor.cached() && !descriptor.substructOnly())
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
        m_poolChanged |= ImGui::InputTextWithHint("##containerFilter", "<none>", m_containerFilterBuffer.data(), m_containerFilterBuffer.size());
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            m_containerFilterBuffer.assign(m_containerFilterBuffer.size(), '\0');
            m_poolChanged = true;
        }

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

        ImGui::SameLine();
        if (ImGui::Checkbox("Internal", &m_showInternal))
        {
            m_poolChanged = true;
        }

        if (m_poolChanged)
        {
            m_containerViewsFiltered.clear();
            std::string_view containerFilter = m_containerFilterBuffer.data();
            std::regex regex{ containerFilter.data() };

            std::copy_if(m_containerViews.begin(), m_containerViews.end(), std::back_inserter(m_containerViewsFiltered), [&](const auto& containerView)
            {
                const dots::type::StructDescriptor<>& descriptor = containerView->container().descriptor();
                return (!descriptor.internal() || m_showInternal) && (containerFilter.empty() || std::regex_search(descriptor.name(), regex));
            });
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
            bool containerOpen = containerView->renderBegin();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", containerView->container().size());

            if (containerOpen)
            {
                ImGui::TableNextColumn();
                containerView->renderEnd();
            }
        }

        ImGui::EndTable();
    }
}
