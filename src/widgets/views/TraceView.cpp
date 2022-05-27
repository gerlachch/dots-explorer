#include <widgets/views/TraceView.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <common/Settings.h>
#include <widgets/views/EventView.h>
#include <StructDescriptorData.dots.h>
#include <EnumDescriptorData.dots.h>

TraceView::TraceView() :
    m_traceIndex(0),
    m_filtersChanged(true),
    m_filterSettingsInitialized(false),
    m_filterSettings{ Settings::Register<FilterSettings>() },
    m_pageScrollTotalTime(0.0f),
    m_pageScrollDeltaTime(0.0f)
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
            item->setFilterTargets(m_filterSettings.targets);

            if (item->isFiltered(m_filterMatcher, m_filterSettings))
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
    m_filterSettings.activeFilter.constructOrValue();
    m_filterSettings.activeFilter->expression.constructOrValue();
    m_filterSettings.activeFilter->description.constructOrValue();
    m_filterSettings.activeFilter->matchCase.constructOrValue();
    m_filterExpressionEdit.emplace(m_filterSettings.activeFilter);

    m_filterSettings.types.constructOrValue();
    m_filterSettings.types->internal.constructOrValue();
    m_filterSettings.types->uncached.constructOrValue();
    m_filterSettings.types->empty.constructOrValue();

    m_filterSettings.targets.constructOrValue();
    m_filterSettings.targets->type.constructOrValue(true);
    m_filterSettings.targets->publisher.constructOrValue(true);
    m_filterSettings.targets->instance.constructOrValue(true);

    // ensure filters are valid
    {
        dots::vector_t<Filter>& filters = m_filterSettings.storedFilters.constructOrValue();
        filters.erase(std::remove_if(filters.begin(), filters.end(), [](const Filter& filter){ return !filter._hasProperties(filter._properties()); }), filters.end());

        if (auto& selectedFilter = m_filterSettings.selectedFilter; selectedFilter.isValid() && *selectedFilter >= filters.size())
        {
            selectedFilter.destroy();
        }
    }
}

void TraceView::applyFilters()
{
    try
    {
        FilterMatcher filterMatcher{ m_filterSettings.activeFilter };
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
                m_filterSettings.selectedFilter.destroy();
            }
            ImGui::PopItemWidth();
        }

        // render filter list
        {
            dots::vector_t<Filter>& filters = m_filterSettings.storedFilters;
            auto& selectedFilter = m_filterSettings.selectedFilter;

            ImGui::SameLine(0, 0);

            if (ImGui::BeginCombo("##Filters", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
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

                // render filter rules
                {
                    ImGui::TextUnformatted("Filter By:");
                    ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

                    if (ImGui::MenuItem("Type", nullptr, &*m_filterSettings.targets->type))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Publisher", nullptr, &*m_filterSettings.targets->publisher))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(m_filterSettings.targets); }
                        m_filtersChanged = true;
                    }

                    if (ImGui::MenuItem("Instance", nullptr, &*m_filterSettings.targets->instance))
                    {
                        for (const auto& item : m_items){ item->setFilterTargets(m_filterSettings.targets); }
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
                        m_filterExpressionEdit = FilterExpressionEdit{ m_filterSettings.activeFilter };
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

            if (ImGuiExt::ToggleButton("Aa", m_filterSettings.activeFilter->matchCase, "Match case"))
            {
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
                    m_filterExpressionEdit = FilterExpressionEdit{ m_filterSettings.activeFilter };
                    m_filtersChanged = true;
                    m_filterSettings.selectedFilter.destroy();
                }
            }
        }

        // render 'Internal' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Internal", &*m_filterSettings.types->internal))
            {
                m_filtersChanged = true;
            }
        }

        // render 'Uncached' filter option checkbox
        {
            ImGui::SameLine();

            if (ImGui::Checkbox("Uncached", &*m_filterSettings.types->uncached))
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
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_ScrollX        |
        ImGuiTableFlags_ScrollY        |
        ImGuiTableFlags_Reorderable    |
        ImGuiTableFlags_Hideable
    ;

    const TraceItem* editItem = nullptr;
    std::shared_ptr<TraceItem> discardUntilItem;
    bool discardAll = false;

    if (ImGui::BeginTable("EventTrace", 8, TableFlags, ImGui::GetContentRegionAvail()))
    {
        // render event list headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("No.");
        ImGui::TableSetupColumn("Published At");
        ImGui::TableSetupColumn("Published By");
        ImGui::TableSetupColumn("Operation");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultHide);
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
                    EventView{ item.metadataModel(), item.publishedInstanceModel(), item.updatedInstanceModel() }.render();
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
                        const StructDescriptorModel& descriptorModel = item.publishedInstanceModel().descriptorModel();

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

            auto scroll_if_down = [this](ImGuiKey key, float scrollY)
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

            // scroll up and down respectively
            float singleItemY = ImGui::GetItemRectSize().y;
            float clippedItemsY = static_cast<float>(std::max(0, clipper.DisplayEnd - clipper.DisplayStart - 1)) * singleItemY;
            scroll_if_down(ImGuiKey_UpArrow, -singleItemY);
            scroll_if_down(ImGuiKey_DownArrow, +singleItemY);
            scroll_if_down(ImGuiKey_PageUp, -clippedItemsY);
            scroll_if_down(ImGuiKey_PageDown, +clippedItemsY);
        }

        ImGui::EndTable();
    }

    // render struct edit
    {
        if (editItem != nullptr)
        {
            const StructRefModel& structRefModel = editItem->publishedInstanceModel();
            m_publishDialog.emplace(structRefModel.descriptorModel(), structRefModel.instance());
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
