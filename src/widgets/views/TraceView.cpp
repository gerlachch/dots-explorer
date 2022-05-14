#include <widgets/views/TraceView.h>
#include <imgui.h>
#include <common/Settings.h>
#include <widgets/views/StructView.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

TraceView::TraceView() :
    m_traceIndex(0),
    m_eventFilterBuffer(256, '\0'),
    m_filtersChanged(true),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() }
{
    m_subscriptions.emplace_back(dots::subscribe<StructDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<EnumDescriptorData>([](auto&){}));
    m_subscriptions.emplace_back(dots::subscribe<dots::type::StructDescriptor<>>({ &TraceView::update, this }));
}

void TraceView::update(const dots::type::StructDescriptor<>& descriptor)
{
    if (!descriptor.substructOnly())
    {
        m_subscriptions.emplace_back(dots::subscribe(descriptor, [this](const dots::Event<>& event)
        {
            StructDescriptorModel& descriptorModel = m_descriptorModels.try_emplace(&event.descriptor(), event.descriptor()).first->second;
            auto& item = m_items.emplace_back(std::make_shared<TraceItem>(++m_traceIndex, descriptorModel, m_publisherModel, event));

            if (applyFilter(*item))
            {
                m_itemsFiltered.emplace_back(item);
            }
        }));
    }
}

void TraceView::render()
{
    if (!m_filterSettingsInitialized)
    {
        m_filterSettingsInitialized = true;
        initFilterSettings();
        ImGui::SetKeyboardFocusHere();
    }

    renderFilterArea();
    renderEventList();
}

void TraceView::initFilterSettings()
{
    if (m_filterSettings.regexFilter.isValid())
    {
        const std::string& regexFilter = m_filterSettings.regexFilter;
        m_eventFilterBuffer.assign(std::max(regexFilter.size(), m_eventFilterBuffer.size()), '\0');
        std::copy(regexFilter.begin(), regexFilter.end(), m_eventFilterBuffer.begin());
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

bool TraceView::applyFilter(const TraceItem& item)
{
    std::string_view eventFilter = m_eventFilterBuffer.data();
    const dots::type::StructDescriptor<>& descriptor = item.structModel().descriptorModel().descriptor();

    if (descriptor.internal() && !*m_filterSettings.showInternal)
    {
        return false;
    }
    else if (!descriptor.cached() && !*m_filterSettings.showUncached)
    {
        return false;
    }
    else
    {
        return eventFilter.empty() || (m_regex != std::nullopt && std::regex_search(descriptor.name(), *m_regex));
    }
}

void TraceView::applyFilters()
{
    std::string_view eventFilter = m_eventFilterBuffer.data();
    m_filterSettings.regexFilter = eventFilter;

    std::regex_constants::syntax_option_type regexFlags = std::regex_constants::ECMAScript;

    if (!m_filterSettings.matchCase)
    {
        regexFlags |= std::regex_constants::icase;
    }

    try
    {
        std::regex regex{ eventFilter.data(), regexFlags };
        m_regex.emplace(std::move(regex));
        m_itemsFiltered.clear();

        std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(m_itemsFiltered), [&](const auto& item)
        {
            return applyFilter(*item);
        });
    }
    catch (...)
    {
    }
}

void TraceView::renderFilterArea()
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
            if (ImGui::InputTextWithHint("##typeFilter", "<none>", m_eventFilterBuffer.data(), m_eventFilterBuffer.size()))
            {
                m_filtersChanged = true;
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
                        m_eventFilterBuffer.assign(std::max(regexFilter.size(), m_eventFilterBuffer.size()), '\0');
                        std::copy(regexFilter.begin(), regexFilter.end(), m_eventFilterBuffer.begin());
                        m_filtersChanged = true;
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
                m_filtersChanged = true;
            }

            ImGui::PopStyleColor();

            ImGuiExt::TooltipLastHoveredItem("Match case");
        }

        // render 'Clear' button
        {
            ImGui::SameLine();
            constexpr char ClearLabel[] = "Clear";

            if (m_eventFilterBuffer.front() == '\0')
            {
                ImGui::BeginDisabled();
                ImGui::Button(ClearLabel);
                ImGui::EndDisabled();
            }
            else
            {
                if (ImGui::Button(ClearLabel))
                {
                    m_eventFilterBuffer.assign(m_eventFilterBuffer.size(), '\0');
                    m_filtersChanged = true;
                    m_filterSettings.selectedFilter.destroy();
                }
            }
        }

        // render 'Internal' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Internal", &*m_filterSettings.showInternal))
            {
                m_filtersChanged = true;
            }
        }

        // render 'Uncached' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Uncached", &*m_filterSettings.showUncached))
            {
                m_filtersChanged = true;
            }
        }

        // apply filters to event list
        if (m_filtersChanged)
        {
            applyFilters();
            m_filtersChanged = false;
        }

        // render filtered events hint label
        {
            ImGui::SameLine();
            if (m_itemsFiltered.size() == 1)
            {
                ImGui::TextDisabled("(showing 1 events)");
            }
            else
            {
                ImGui::TextDisabled("(showing %zu events)", m_itemsFiltered.size());
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

void TraceView::renderEventList()
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
        ImGuiTableFlags_ScrollX        |
        ImGuiTableFlags_ScrollY        |
        ImGuiTableFlags_Reorderable    |
        ImGuiTableFlags_Hideable
    ;

    const TraceItem* editItem = nullptr;
    std::shared_ptr<TraceItem> discardUntilItem;
    bool discardAll = false;

    if (ImGui::BeginTable("EventTrace", 6, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // render event list headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("No.");
        ImGui::TableSetupColumn("Published At");
        ImGui::TableSetupColumn("Published By");
        ImGui::TableSetupColumn("Operation");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Published Instance");
        ImGui::TableHeadersRow();

        // render event list
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(m_itemsFiltered.size()));

        while (clipper.Step())
        {
            for (int itemIndex = clipper.DisplayStart; itemIndex < clipper.DisplayEnd; ++itemIndex)
            {
                // render event
                {
                    TraceItem& item = *m_itemsFiltered[itemIndex];
                    bool hoverCondition = ImGui::GetIO().KeyAlt;
                    item.render(hoverCondition);

                    // this dummy selectable is currently required for the context menu to work
                    ImGui::SameLine();
                    ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);
                }

                // render quick info tooltip
                if (const TraceItem& item = *m_itemsFiltered[itemIndex]; item.isHovered())
                {
                    ImGui::BeginTooltip();
                    StructView structView{ item.metadataModel(), item.structModel() };
                    structView.render();
                    ImGui::EndTooltip();

                    // open instance in struct edit when clicked
                    if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
                    {
                        editItem = &item;
                    }
                }

                // render context menu
                {
                    const TraceItem& item = *m_itemsFiltered[itemIndex];

                    if (ImGui::BeginPopupContextItem(item.widgetId()))
                    {
                        const StructDescriptorModel& descriptorModel = item.structModel().descriptorModel();

                        ImGuiExt::TextColored(descriptorModel.declarationText());
                        ImGui::Separator();

                        if (ImGui::MenuItem(descriptorModel.descriptor().cached() ? "View/Update" : "View/Publish"))
                        {
                            editItem = &item;
                        }

                        if (ImGui::MenuItem("Discard Until [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                        {
                            discardUntilItem = m_itemsFiltered[itemIndex];
                        }

                        if (ImGui::MenuItem("Discard All [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                        {
                            discardAll = true;
                        }

                        ImGui::EndPopup();
                    }
                }
            }
        }

        // auto scroll when view is at the bottom
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndTable();
    }

    // render struct edit
    {
        if (editItem != nullptr)
        {
            const StructModel& structModel = editItem->structModel();
            m_structEdit.emplace(structModel.descriptorModel(), structModel.instance());
        }

        if (m_structEdit != std::nullopt && !m_structEdit->render())
        {
            m_structEdit = std::nullopt;
        }
    }

    // discard items
    {
        if (discardUntilItem != nullptr)
        {
            auto comp = [](const auto& lhs, const auto& rhs)
            {
                return lhs->index() < rhs->index();
            };

            if (auto it = std::lower_bound(m_items.begin(), m_items.end(), discardUntilItem, comp); it != m_items.end())
            {
                m_items.erase(m_items.begin(), it);
            }

            if (auto it = std::lower_bound(m_itemsFiltered.begin(), m_itemsFiltered.end(), discardUntilItem, comp); it != m_itemsFiltered.end())
            {
                m_itemsFiltered.erase(m_itemsFiltered.begin(), it);
            }
        }

        if (discardAll)
        {
            m_traceIndex = 0;
            m_items.clear();
            m_itemsFiltered.clear();
        }
    }
}
