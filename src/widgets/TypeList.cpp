#include <widgets/TypeList.h>
#include <string_view>
#include <regex>
#include <imgui.h>
#include <widgets/StructList.h>
#include <common/Settings.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

TypeList::TypeList() :
    m_typeFilterBuffer(256, '\0'),
    m_typesChanged(false),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() }
{
    m_subscriptions.emplace_back(dots::subscribe<StructDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<EnumDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor<>>({ &TypeList::update, this }));
}

void TypeList::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (!descriptor.substructOnly())
    {
        StructList& structList = *m_typeList.emplace_back(std::make_shared<StructList>(descriptor));
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

void TypeList::render()
{
    // init filter settings
    if (!m_filterSettingsInitialized)
    {
        m_filterSettingsInitialized = true;

        if (m_filterSettings.regexFilter.isValid())
        {
            const std::string& regexFilter = m_filterSettings.regexFilter;
            m_typeFilterBuffer.assign(std::max(regexFilter.size(), m_typeFilterBuffer.size()), '\0');
            std::copy(regexFilter.begin(), regexFilter.end(), m_typeFilterBuffer.begin());
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

            if (auto& selectedFilter = m_filterSettings.selectedFilter; selectedFilter.isValid() && (*selectedFilter < 0 || *selectedFilter >= filters.size()))
            {
                selectedFilter.destroy();
            }
        }

        ImGui::SetKeyboardFocusHere();
    }

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
            if (ImGui::InputTextWithHint("##typeFilter", "<none>", m_typeFilterBuffer.data(), m_typeFilterBuffer.size()))
            {
                m_typesChanged = true;
                m_filterSettings.selectedFilter.destroy();
            }
            ImGui::PopItemWidth();

            // render filter hint tooltip
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("Types can be filtered by specifying substrings or ECMAScript regular expressions.");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
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
                        const std::string& regexFilter = filters[selectedFilter].regex;
                        m_typeFilterBuffer.assign(std::max(regexFilter.size(), m_typeFilterBuffer.size()), '\0');
                        std::copy(regexFilter.begin(), regexFilter.end(), m_typeFilterBuffer.begin());
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

            if (m_typeFilterBuffer.front() == '\0')
            {
                ImGui::BeginDisabled();
                ImGui::Button(ClearLabel);
                ImGui::EndDisabled();
            }
            else
            {
                if (ImGui::Button(ClearLabel))
                {
                    m_typeFilterBuffer.assign(m_typeFilterBuffer.size(), '\0');
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
            m_typeListFiltered.clear();
            std::string_view typeFilter = m_typeFilterBuffer.data();
            m_filterSettings.regexFilter = typeFilter;

            std::regex_constants::syntax_option_type regexFlags = std::regex_constants::ECMAScript;

            if (!m_filterSettings.matchCase)
            {
                regexFlags |= std::regex_constants::icase;
            }

            std::regex regex{ typeFilter.data(), regexFlags };

            std::copy_if(m_typeList.begin(), m_typeList.end(), std::back_inserter(m_typeListFiltered), [&](const auto& structList)
            {
                const dots::type::StructDescriptor<>& descriptor = structList->container().descriptor();

                if (descriptor.internal() && !*m_filterSettings.showInternal)
                {
                    return false;
                }
                else if (!descriptor.cached() && !*m_filterSettings.showUncached)
                {
                    return false;
                }
                else if (descriptor.cached() && structList->container().empty() && !*m_filterSettings.showEmpty)
                {
                    return false;
                }
                else
                {
                    return typeFilter.empty() || std::regex_search(descriptor.name(), regex);
                }
            });
        }

        // render filtered types hint label
        {
            ImGui::SameLine();
            if (m_typeListFiltered.size() == 1)
            {
                ImGui::TextDisabled("(showing 1 type)");
            }
            else
            {
                ImGui::TextDisabled("(showing %zu types)", m_typeListFiltered.size());
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

    // type list
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
           ImGuiTableFlags_Sortable      |
           ImGuiTableFlags_Hideable
        ;
    
        if (ImGui::BeginTable("Cached Types", 4, TableFlags, ImGui::GetContentRegionAvail()))
        {
            // render type list headers
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Activity", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableSetupColumn("Activity [dot]", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            // sort type list
            if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_typesChanged || sortSpecs->SpecsDirty)
            {
                std::sort(m_typeListFiltered.begin(), m_typeListFiltered.end(), [sortSpecs](const auto& lhs, const auto& rhs)
                {
                    return lhs->less(*sortSpecs, *rhs);
                });

                m_typesChanged = false;
                sortSpecs->SpecsDirty = false;
            }

            // render type list
            for (auto& structList : m_typeListFiltered) 
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
}
