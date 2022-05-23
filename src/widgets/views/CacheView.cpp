#include <widgets/views/CacheView.h>
#include <string_view>
#include <imgui.h>
#include <widgets/views/StructList.h>
#include <common/Settings.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

CacheView::CacheView() :
    m_typesChanged(false),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() }
{
    m_subscriptions.emplace_back(dots::subscribe<StructDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<EnumDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor<>>({ &CacheView::update, this }));
}

void CacheView::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (!descriptor.substructOnly())
    {
        StructList& structList = *m_cacheList.emplace_back(std::make_shared<StructList>(descriptor, m_publisherModel));
        m_typesChanged = true;

        m_subscriptions.emplace_back(dots::subscribe(descriptor, [this, &structList](const dots::Event<>& event)
        {
            structList.update(event);

            if (!m_filterSettings.showEmpty == true &&
                ((event.isCreate() && structList.container().size() == 1) || 
                (event.isRemove() && structList.container().empty())))
            {
                m_typesChanged = true;
            }
        }));
    }
}

void CacheView::render()
{
    // init filter settings
    if (!m_filterSettingsInitialized)
    {
        m_filterSettingsInitialized = true;
        initFilterSettings();
        ImGui::SetKeyboardFocusHere();
    }

    renderFilterArea();
    renderCacheList();
}

void CacheView::initFilterSettings()
{
    if (m_filterSettings.regexFilter.isValid())
    {
        m_filterEdit = std::string_view{ *m_filterSettings.regexFilter };
    }
    else
    {
        m_filterSettings.regexFilter.construct();
    }

    m_filterSettings.showInternal.constructOrValue();
    m_filterSettings.showUncached.constructOrValue();
    m_filterSettings.showEmpty.constructOrValue();
    m_filterSettings.matchCase.constructOrValue();

    // ensure filters are valid
    {
        dots::vector_t<Filter>& filters = m_filterSettings.filters.constructOrValue();
        filters.erase(std::remove_if(filters.begin(), filters.end(), [](const Filter& filter){ return !filter._hasProperties(filter._properties()); }), filters.end());

        if (auto& selectedFilter = m_filterSettings.selectedFilter; selectedFilter.isValid() && *selectedFilter >= filters.size())
        {
            selectedFilter.destroy();
        }
    }
}

bool CacheView::applyFilter(const StructList& structList)
{
    std::string_view typeFilter = m_filterEdit.text().first;
    const dots::type::StructDescriptor<>& descriptor = structList.container().descriptor();

    if (descriptor.internal() && !*m_filterSettings.showInternal)
    {
        return false;
    }
    else if (!descriptor.cached() && !*m_filterSettings.showUncached)
    {
        return false;
    }
    else if (descriptor.cached() && structList.container().empty() && !*m_filterSettings.showEmpty)
    {
        return false;
    }
    else
    {
        return typeFilter.empty() || (m_regex != std::nullopt && m_regex->search(descriptor.name()));
    }
}

void CacheView::applyFilters()
{
    std::string_view typeFilter = m_filterEdit.text().first;
    m_filterSettings.regexFilter = typeFilter;

    try
    {
        Regex regex{ typeFilter.data(), m_filterSettings.matchCase };
        m_regex.emplace(std::move(regex));
        m_cacheListFiltered.clear();

        std::copy_if(m_cacheList.begin(), m_cacheList.end(), std::back_inserter(m_cacheListFiltered), [&](const auto& structList)
        {
            return applyFilter(*structList);
        });
    }
    catch (...)
    {
    }
}

void CacheView::renderFilterArea()
{
    bool openFilterSettingsEdit = false;
    Filter* editFilter = nullptr;

    // render control area
    {
        // render filter input
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Filter");

            if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F, false))
            {
                ImGui::SetKeyboardFocusHere();
            }

            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f - 19);
            if (m_filterEdit.render())
            {
                m_typesChanged = true;
                m_filterSettings.selectedFilter.destroy();
            }
            ImGui::PopItemWidth();
            ImGuiExt::TooltipLastHoveredItem("Types can be filtered by specifying substrings or ECMAScript regular expressions.");
        }

        // render filter list
        {
            dots::vector_t<Filter>& filters = m_filterSettings.filters;
            auto& selectedFilter = m_filterSettings.selectedFilter;

            ImGui::SameLine(0, 0);

            if (ImGui::BeginCombo("##Filters", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft))
            {
                if (ImGui::Selectable("<New>"))
                {
                    openFilterSettingsEdit = true;
                }

                if (selectedFilter.isValid())
                {
                    if (ImGui::Selectable("<Edit>"))
                    {
                        openFilterSettingsEdit = true;
                        editFilter = &filters[selectedFilter];
                    }

                    if (ImGui::Selectable("<Remove>"))
                    {
                        filters.erase(filters.begin() + selectedFilter);

                        if (*selectedFilter > filters.size())
                        {
                            --*selectedFilter;
                        }
                        else
                        {
                            selectedFilter.destroy();
                        }
                    }
                }

                ImGui::Separator();

                uint32_t i = 0;

                for (Filter& filter : filters)
                {
                    if (ImGui::Selectable(filter.description->data(), selectedFilter == i) && selectedFilter != i)
                    {
                        selectedFilter = i;
                        m_filterEdit = std::string_view{ *filters[selectedFilter].regex };
                        m_typesChanged = true;
                    }

                    ++i;
                }

                ImGui::EndCombo();
            }
        }

        // render 'Match case' button
        {
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[*m_filterSettings.matchCase ? ImGuiCol_ButtonActive : ImGuiCol_Button]);

            if (ImGui::Button("Aa"))
            {
                m_filterSettings.matchCase = !*m_filterSettings.matchCase;
                m_typesChanged = true;
            }

            ImGui::PopStyleColor();

            ImGuiExt::TooltipLastHoveredItem("Match case");
        }

        // render 'Clear' button
        {
            ImGui::SameLine();
            constexpr char ClearLabel[] = "Clear";

            if (m_filterEdit.text().first.empty())
            {
                ImGui::BeginDisabled();
                ImGui::Button(ClearLabel);
                ImGui::EndDisabled();
            }
            else
            {
                if (ImGui::Button(ClearLabel))
                {
                    m_filterEdit = {};
                    m_typesChanged = true;
                    m_filterSettings.selectedFilter.destroy();
                }
            }
        }

        // render 'Internal' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Internal", &*m_filterSettings.showInternal))
            {
                m_typesChanged = true;
            }
        }

        // render 'Uncached' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Uncached", &*m_filterSettings.showUncached))
            {
                m_typesChanged = true;
            }
        }

        // render 'Empty' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Empty", &*m_filterSettings.showEmpty))
            {
                m_typesChanged = true;
            }
        }

        // apply filters to type list
        if (m_typesChanged)
        {
            applyFilters();
        }

        // render filtered types hint label
        {
            ImGui::SameLine();
            if (m_cacheListFiltered.size() == 1)
            {
                ImGui::TextDisabled("(showing 1 type)");
            }
            else
            {
                ImGui::TextDisabled("(showing %zu types)", m_cacheListFiltered.size());
            }
        }
    }

    // render filter settings edit
    {
        if (openFilterSettingsEdit)
        {
            m_filterSettingsEdit.emplace(m_filterSettings, editFilter);
        }

        if (m_filterSettingsEdit != std::nullopt && !m_filterSettingsEdit->render())
        {
            m_filterSettingsEdit = std::nullopt;
        }
    }
}

void CacheView::renderCacheList()
{
    constexpr ImGuiTableFlags TableFlags = 
        ImGuiTableFlags_Borders       |
        ImGuiTableFlags_ScrollY       |
        ImGuiTableFlags_Sortable      |
        ImGuiTableFlags_Hideable
    ;

    if (ImGui::BeginTable("Cached Types", 4, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // render cache list headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Activity", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Activity [dot]", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // sort cache list
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_typesChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_cacheListFiltered.begin(), m_cacheListFiltered.end(), [sortSpecs](const auto& lhs, const auto& rhs)
            {
                return lhs->less(*sortSpecs, *rhs);
            });

            m_typesChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render cache list
        for (auto& structList : m_cacheListFiltered)
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            bool structListOpen = structList->renderBegin();

            if (ImGui::TableNextColumn())
            {
                structList->renderActivity();
            }

            if (ImGui::TableNextColumn())
            {
                structList->renderActivityDot();
            }

            if (ImGui::TableNextColumn())
            {
                ImGui::Text("%zu", structList->container().size());
            }

            if (structListOpen)
            {
                ImGui::TableNextColumn();
                structList->renderEnd();
            }
        }

        ImGui::EndTable();
    }
}
