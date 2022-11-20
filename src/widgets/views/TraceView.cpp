#include <widgets/views/TraceView.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <common/Settings.h>
#include <widgets/views/EventView.h>

TraceView::TraceView(TransceiverModel& transceiverModel) :
    m_traceIndex(0),
    m_filtersChanged(true),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() },
    m_pageScrollTotalTime(0.0f),
    m_pageScrollDeltaTime(0.0f),
    m_sidewaysScrollTotalTime(0.0f),
    m_sidewaysScrollDeltaTime(0.0f)
{
    transceiverModel.subscribe([this](const EventModel& eventModel)
    {
        auto& item = m_items.emplace_back(std::make_shared<TraceItem>(eventModel));
        item->setFilterTargets(*m_filterSettings.targets);

        if (item->isFiltered(m_filterMatcher, m_filterSettings))
        {
            m_itemsFiltered.emplace_back(item);
        }
    });
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
    m_filterExpressionEdit.emplace(*m_filterSettings.activeFilter);

    if (!*m_filterSettings.targets->initialized)
    {
        m_filterSettings.targets->initialized = true;
        m_filterSettings.targets->publishedAt = true;
        m_filterSettings.targets->publishedBy = true;
        m_filterSettings.targets->operation = true;
        m_filterSettings.targets->type = true;
        m_filterSettings.targets->instance = true;
    }

    // ensure filters are valid
    {
        dots::vector_t<Filter>& filters = *m_filterSettings.storedFilters;

        if (auto& selectedFilter = m_filterSettings.selectedFilter; *selectedFilter >= filters.size())
        {
            selectedFilter = NoFilterSelected;
        }
    }
}

void TraceView::applyFilters()
{
    try
    {
        FilterMatcher filterMatcher{ *m_filterSettings.activeFilter };
        m_filterMatcher.emplace(std::move(filterMatcher));
        m_itemsFiltered.clear();

        std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(m_itemsFiltered), [&](const auto& item)
        {
            return item->isFiltered(m_filterMatcher, m_filterSettings);
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
            if (m_filterExpressionEdit->render())
            {
                m_filtersChanged = true;
                m_filterSettings.selectedFilter = NoFilterSelected;
            }
            ImGui::PopItemWidth();
        }

        // render filter list
        {
            dots::vector_t<Filter>& filters = *m_filterSettings.storedFilters;
            uint32_t& selectedFilter = *m_filterSettings.selectedFilter;

            ImGui::SameLine(0, 0);

            if (ImGui::BeginCombo("##Filters", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
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

                // render filter rules
                {
                    ImGui::TextUnformatted("Filter By:");
                    ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

                    if (ImGui::MenuItem("Published At", nullptr, &*m_filterSettings.targets->publishedAt))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(*m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Published By", nullptr, &*m_filterSettings.targets->publishedBy))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(*m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Operation", nullptr, &*m_filterSettings.targets->operation))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(*m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Type", nullptr, &*m_filterSettings.targets->type))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(*m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Instance", nullptr, &*m_filterSettings.targets->instance))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(*m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    ImGui::PopItemFlag();
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
                        m_filtersChanged = true;
                    }

                    ++i;
                }

                ImGui::EndCombo();
            }
        }

        // render filter expression options
        {
            ImGui::SameLine();
            m_filtersChanged |= ImGuiExt::ToggleButton("Aa", *m_filterSettings.activeFilter->matchCase, "Match case");

            ImGui::SameLine();
            if (ImGuiExt::ToggleButton("Re", *m_filterSettings.activeFilter->regex, "Interpret expression as a regular expression instead of a quick filter."))
            {
                m_filterExpressionEdit = FilterExpressionEdit{ *m_filterSettings.activeFilter };
                m_filtersChanged = true;
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
                    m_filtersChanged = true;
                    m_filterSettings.selectedFilter = NoFilterSelected;
                }
            }
        }

        // render type filter option checkboxes
        {
            ImGui::SameLine();
            m_filtersChanged |= ImGui::Checkbox("Internal", &*m_filterSettings.types->internal);
            
            ImGui::SameLine();
            m_filtersChanged |= ImGui::Checkbox("Uncached", &*m_filterSettings.types->uncached);
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
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_ScrollX        |
        ImGuiTableFlags_ScrollY        |
        ImGuiTableFlags_Reorderable    |
        ImGuiTableFlags_Hideable
    ;

    const TraceItem* editItem = nullptr;
    std::shared_ptr<TraceItem> discardUntilItem;
    bool discardAll = false;

    if (ImGui::BeginTable("EventTrace", 7, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // render event list headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("No.");
        ImGui::TableSetupColumn("Published At");
        ImGui::TableSetupColumn("Published By");
        ImGui::TableSetupColumn("Operation");
        ImGui::TableSetupColumn("Published Instance");
        ImGui::TableSetupColumn("Updated Instance", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("<intentionally-empty> ", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoHeaderLabel);
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
                    ImGui::TableNextColumn();
                    ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);
                }

                // render quick info tooltip
                if (const TraceItem& item = *m_itemsFiltered[itemIndex]; item.isHovered())
                {
                    ImGui::BeginTooltip();
                    EventView{ item.model() }.render();
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
                        const StructDescriptorModel& descriptorModel = item.model().publishedInstanceModel().descriptorModel();

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

        // process scroll input
        {
            if (ImGui::IsKeyPressed(ImGuiKey_End, false))
            {
                // jump to bottom
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                ImGui::SetScrollHereY(1.0f);
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Home, false))
            {
                // jump to top
                ImGui::SetScrollY(0.0f);
            }

            // scroll up and down respectively
            auto scroll_y_if_down = [this](ImGuiKey key, float scrollY)
            {
                if (ImGui::IsKeyDown(key))
                {
                    float pageScrollTimeMultiplier = 1.0f + std::floor(m_pageScrollTotalTime / 10.0f);

                    if (ImGui::IsKeyPressed(key) || (m_pageScrollTotalTime >= ImGui::GetIO().KeyRepeatDelay && m_pageScrollDeltaTime >= 1.0f / 60.0f))
                    {
                        ImGui::SetScrollY(ImGui::GetScrollY() + scrollY * pageScrollTimeMultiplier);
                        m_pageScrollDeltaTime = 0.0f;
                    }

                    m_pageScrollTotalTime += ImGui::GetIO().DeltaTime;
                    m_pageScrollDeltaTime += ImGui::GetIO().DeltaTime;
                }
                else if (ImGui::IsKeyReleased(key))
                {
                    m_pageScrollTotalTime = 0.0f;
                    m_pageScrollDeltaTime = 0.0f;
                }
            };
            
            float singleItemY = ImGui::GetItemRectSize().y;
            float clippedItemsY = static_cast<float>(std::max(0, clipper.DisplayEnd - clipper.DisplayStart - 1)) * singleItemY;
            scroll_y_if_down(ImGuiKey_UpArrow, -singleItemY);
            scroll_y_if_down(ImGuiKey_DownArrow, +singleItemY);
            scroll_y_if_down(ImGuiKey_PageUp, -clippedItemsY);
            scroll_y_if_down(ImGuiKey_PageDown, +clippedItemsY);

            // scroll left and right respectively
            auto scroll_x_if_down = [this](ImGuiKey key, float scrollX)
            {
                if (ImGui::IsKeyDown(key))
                {
                    float scrollXScrollTimeMultiplier = 1.0f + std::floor(m_sidewaysScrollTotalTime / 10.0f);

                    if (ImGui::IsKeyPressed(key) || (m_sidewaysScrollTotalTime >= ImGui::GetIO().KeyRepeatDelay && m_sidewaysScrollDeltaTime >= 1.0f / 60.0f))
                    {
                        ImGui::SetScrollX(ImGui::GetScrollX() + scrollX * scrollXScrollTimeMultiplier);
                        m_sidewaysScrollDeltaTime = 0.0f;
                    }

                    m_sidewaysScrollTotalTime += ImGui::GetIO().DeltaTime;
                    m_sidewaysScrollDeltaTime += ImGui::GetIO().DeltaTime;
                }
                else if (ImGui::IsKeyReleased(key))
                {
                    m_sidewaysScrollTotalTime = 0.0f;
                    m_sidewaysScrollDeltaTime = 0.0f;
                }
            };

            float stepSizeX = singleItemY;
            scroll_x_if_down(ImGuiKey_LeftArrow, -stepSizeX);
            scroll_x_if_down(ImGuiKey_RightArrow, +stepSizeX);
        }

        ImGui::EndTable();
    }

    // render struct edit
    {
        if (editItem != nullptr)
        {
            const StructModel& structModel = editItem->model().publishedInstanceModel();
            m_publishDialog.emplace(StructModel{ structModel.descriptorModel(), structModel.instance() });
        }

        if (m_publishDialog != std::nullopt && !m_publishDialog->render())
        {
            m_publishDialog = std::nullopt;
        }
    }

    // discard items
    {
        if (discardUntilItem != nullptr)
        {
            auto comp = [](const auto& lhs, const auto& rhs)
            {
                return lhs->model().index() < rhs->model().index();
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
