#include <widgets/views/CacheView.h>
#include <string_view>
#include <imgui.h>
#include <widgets/views/StructList.h>
#include <common/Settings.h>

CacheView::CacheView(TransceiverModel& transceiverModel) :
    m_typesChanged(false),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() }
{
    transceiverModel.subscribe([this, &transceiverModel](const StructDescriptorModel& descriptorModel)
    {
        StructList& structList = *m_cacheList.emplace_back(std::make_shared<StructList>(descriptorModel));
        m_typesChanged = true;

        transceiverModel.subscribe(descriptorModel, [this, &structList](const EventModel& eventModel)
        {
            structList.update(eventModel);

            if (!*m_filterSettings.types->empty &&
                ((eventModel.metadataModel().lastOperation() == DotsMt::create && structList.size() == 1) || 
                (eventModel.metadataModel().lastOperation() == DotsMt::remove && structList.size() == 0)))
            {
                m_typesChanged = true;
            }
        });
    });
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
    m_filterExpressionEdit.emplace(*m_filterSettings.activeFilter);

    // ensure filters are valid
    {
        dots::vector_t<Filter>& filters = *m_filterSettings.storedFilters;

        if (auto& selectedFilter = m_filterSettings.selectedFilter; *selectedFilter >= filters.size())
        {
            selectedFilter = NoFilterSelected;
        }
    }
}

void CacheView::applyFilters()
{
    try
    {
        FilterMatcher filterMatcher{ *m_filterSettings.activeFilter };
        m_filterMatcher.emplace(std::move(filterMatcher));
        m_cacheListFiltered.clear();

        std::copy_if(m_cacheList.begin(), m_cacheList.end(), std::back_inserter(m_cacheListFiltered), [&](const auto& structList)
        {
            return structList->isFiltered(m_filterMatcher, m_filterSettings);
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
            if (m_filterExpressionEdit->render())
            {
                m_typesChanged = true;
                m_filterSettings.selectedFilter = NoFilterSelected;
            }
            ImGui::PopItemWidth();
        }

        // render filter list
        {
            dots::vector_t<Filter>& filters = *m_filterSettings.storedFilters;
            uint32_t& selectedFilter = *m_filterSettings.selectedFilter;

            ImGui::SameLine(0, 0);

            if (ImGui::BeginCombo("##Filters", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft))
            {
                if (ImGui::Selectable("<New>"))
                {
                    openFilterSettingsEdit = true;
                }

                if (selectedFilter == NoFilterSelected)
                {
                    if (ImGui::Selectable("<Edit>"))
                    {
                        openFilterSettingsEdit = true;
                        editFilter = &filters[selectedFilter];
                    }

                    if (ImGui::Selectable("<Remove>"))
                    {
                        filters.erase(filters.begin() + selectedFilter);

                        if (selectedFilter > filters.size())
                        {
                            --selectedFilter;
                        }
                        else
                        {
                            selectedFilter = NoFilterSelected;
                        }
                    }
                }

                ImGui::Separator();

                ImGui::TextUnformatted("Filters:");
                uint32_t i = 0;

                for (Filter& filter : filters)
                {
                    if (ImGui::Selectable(filter.description->data(), selectedFilter == i) && selectedFilter != i)
                    {
                        selectedFilter = i;
                        m_filterSettings.activeFilter = filters[selectedFilter];
                        m_filterExpressionEdit = FilterExpressionEdit{ *m_filterSettings.activeFilter };
                        m_typesChanged = true;
                    }

                    ++i;
                }

                ImGui::EndCombo();
            }
        }

        // render filter expression options
        {
            ImGui::SameLine();
            m_typesChanged |= ImGuiExt::ToggleButton("Aa", *m_filterSettings.activeFilter->matchCase, "Match case");

            ImGui::SameLine();
            if (ImGuiExt::ToggleButton("Re", *m_filterSettings.activeFilter->regex, "Interpret expression as a regular expression instead of a quick filter."))
            {
                m_filterExpressionEdit = FilterExpressionEdit{ *m_filterSettings.activeFilter };
                m_typesChanged = true;
            }
        }

        // render 'Clear' button
        {
            ImGui::SameLine();
            constexpr char ClearLabel[] = "Clear";

            if (m_filterSettings.activeFilter->expression->empty())
            {
                ImGui::BeginDisabled();
                ImGui::Button(ClearLabel);
                ImGui::EndDisabled();
            }
            else
            {
                if (ImGui::Button(ClearLabel))
                {
                    m_filterSettings.activeFilter->expression->clear();
                    m_filterExpressionEdit = FilterExpressionEdit{ *m_filterSettings.activeFilter };
                    m_typesChanged = true;
                    m_filterSettings.selectedFilter = NoFilterSelected;
                }
            }
        }

        // render type filter option checkboxes
        {
            ImGui::SameLine();
            m_typesChanged |= ImGui::Checkbox("Internal", &*m_filterSettings.types->internal);
            
            ImGui::SameLine();
            m_typesChanged |= ImGui::Checkbox("Uncached", &*m_filterSettings.types->uncached);
            
            ImGui::SameLine();
            m_typesChanged |= ImGui::Checkbox("Empty", &*m_filterSettings.types->empty);
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
                ImGui::Text("%zu", structList->size());
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
