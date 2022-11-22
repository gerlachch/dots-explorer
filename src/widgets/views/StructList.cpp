#include <widgets/views/StructList.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/ImGuiExt.h>
#include <DotsClearCache.dots.h>

StructList::StructList(const StructDescriptorModel& descriptorModel) :
    m_lastPublishedItem(nullptr),
    m_lastPublishedItemTime{ dots::timepoint_t::min() },
    m_lastUpdateDelta(0.0f),
    m_containerChanged(false),
    m_structDescriptorModel{ &descriptorModel }
{
    const dots::type::StructDescriptor& descriptor = m_structDescriptorModel->descriptor();
    std::transform(descriptor.name().begin(), descriptor.name().end(), std::back_inserter(m_typeNameLower), [](unsigned char c){ return std::tolower(c); });

    const auto& propertyPaths = descriptor.propertyPaths();

    if (propertyPaths.size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (const dots::type::PropertyPath& propertyPath : propertyPaths)
        {
            const dots::type::PropertyDescriptor& propertyDescriptor = propertyPath.destination();

            if (propertyDescriptor.valueDescriptor().type() == dots::type::Type::Struct)
                continue;

            std::string pathName;

            for (const auto& element : propertyPath.elements())
            {
                pathName += element.get().name();
                pathName += '.';
            }

            pathName.pop_back();

            m_headers.emplace_back(propertyDescriptor.isKey() ? fmt::format("{} [key]", pathName) : pathName);
        }
    }
    else
    {
        for (const dots::type::PropertyDescriptor& propertyDescriptor : descriptor.propertyDescriptors())
        {
            const std::string& name = propertyDescriptor.name();
            m_headers.emplace_back(propertyDescriptor.isKey() ? fmt::format("{} [key]", name) : name);
        }
    }
}

size_t StructList::size() const
{
    return m_items.size() - m_itemsDirty.size();
}

bool StructList::less(const ImGuiTableSortSpecs& sortSpecs, const StructList& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];

        auto compare = [&sortSpec](const auto& lhs, const auto& rhs)
        {
            if (sortSpec.SortDirection == ImGuiSortDirection_Ascending)
                return std::less{}(lhs, rhs);
            else
                return std::greater{}(lhs, rhs);
        };

        bool less = false;

        switch (sortSpec.ColumnIndex)
        {
            case 0:  less = compare(m_structDescriptorModel->descriptor().name(), m_structDescriptorModel->descriptor().name()); break;
            case 1:  less = compare(m_lastUpdateDelta, other.m_lastUpdateDelta); break;
            case 2:  less = compare(m_lastUpdateDelta, other.m_lastUpdateDelta); break;
            case 3:  less = compare(size(), other.size()); break;
            default: IM_ASSERT(0); break;
        }

        if (less)
            return true;
    }

    return false;
}

bool StructList::isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const
{
    const dots::type::StructDescriptor& descriptor = m_structDescriptorModel->descriptor();

    if (descriptor.internal() && !*filterSettings.types->internal)
        return false;
    else if (!descriptor.cached() && !*filterSettings.types->uncached)
        return false;
    else if (descriptor.cached() && size() == 0 && !*filterSettings.types->empty)
        return false;
    else
    {
        if (filterSettings.activeFilter->expression->empty())
            return true;
        else if (filter == std::nullopt)
            return false;
        else
            return filter->match(*filterSettings.activeFilter->matchCase ? descriptor.name() : m_typeNameLower);
    }
}

void StructList::update(const EventModel& eventModel)
{
    m_containerChanged = true;
    size_t updateIndex = eventModel.descriptorModel().descriptor().cached() ? eventModel.updateIndex() : 0;
    StructItem* item;

    if (auto it = m_itemsStorage.find(updateIndex); it == m_itemsStorage.end())
    {
        item = &m_itemsStorage.emplace(updateIndex, eventModel).first->second;
        m_items.emplace_back(*item);
    }
    else
    {
        item = &it->second;
        item->setModel(eventModel);

        if (item->model().metadataModel().lastOperation() == DotsMt::remove)
            m_itemsDirty.emplace_back(item);
    }

    if (dots::timepoint_t lastPublished = item->model().metadataModel().lastPublished(); lastPublished > m_lastPublishedItemTime)
    {
        m_lastPublishedItem = item;
        m_lastPublishedItemTime = lastPublished;
    }

    m_lastUpdateDelta = 0.0f;
}

bool StructList::renderBegin()
{
    m_lastUpdateDelta += ImGui::GetIO().DeltaTime;

    bool containerOpen = ImGui::TreeNodeEx(m_structDescriptorModel->descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);
    bool openPublishDialog = false;
    std::optional<dots::type::AnyStruct> editInstance;

    // render quick info tooltip for last published struct instance
    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt && !m_items.empty())
    {
        if (m_lastPublishedItem == nullptr)
        {
            m_lastPublishedItem = &std::max_element(m_items.begin(), m_items.end(), [this](const StructItem& lhs, const StructItem& rhs)
            {
                return lhs.model().metadataModel().lastPublished() < rhs.model().metadataModel().lastPublished();
            })->get();
            m_lastPublishedItemTime = m_lastPublishedItem->model().metadataModel().lastPublished();
        }

        m_lastPublishedItem->renderTooltip();

        // open last published instance in struct edit when clicked
        if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
        {
            openPublishDialog = true;
            editInstance = m_lastPublishedItem->model().updatedInstanceModel().instance();
        }
    }

    // context menu
    {
        if (ImGui::BeginPopupContextItem())
        {
            ImGuiExt::TextColored(m_structDescriptorModel->declarationText());
            ImGui::Separator();

            if (ImGui::MenuItem(m_structDescriptorModel->descriptor().cached() ? "Create/Update" : "Publish"))
                openPublishDialog = true;

            if (m_structDescriptorModel->descriptor().cached())
            {
                if (ImGui::MenuItem("Remove All [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                {
                    dots::publish(DotsClearCache{ 
                        .typeNames = { m_structDescriptorModel->descriptor().name() }
                    });
                }
            }

            ImGui::EndPopup();
        }
    }

    // struct edit
    {
        if (openPublishDialog)
        {
            if (editInstance == std::nullopt)
                m_publishDialog.emplace(StructModel{ *m_structDescriptorModel });
            else
                m_publishDialog.emplace(StructModel{ *m_structDescriptorModel, std::move(*editInstance) });
        }

        if (m_publishDialog != std::nullopt && !m_publishDialog->render())
            m_publishDialog = std::nullopt;
    }

    return containerOpen;
}

void StructList::renderEnd()
{
    constexpr ImGuiTableFlags TableFlags = 
        ImGuiTableFlags_Borders        |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_Resizable      |
        ImGuiTableFlags_Sortable       |
        ImGuiTableFlags_SortMulti      |
        ImGuiTableFlags_Hideable
    ;

    const dots::type::StructDescriptor& descriptor = m_structDescriptorModel->descriptor();
    std::optional<dots::type::AnyStruct> editInstance;

    if (ImGui::BeginTable(descriptor.name().data(), StructItem::MetaData::MetaDataSize + static_cast<int>(m_headers.size()), TableFlags))
    {
        // create meta info headers
        {
            ImGui::TableSetupColumn("[META] Last Op.", ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableSetupColumn("[META] Last Published", ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableSetupColumn("[META] Last Published By", ImGuiTableColumnFlags_DefaultHide);
        }

        // create property headers
        for (const std::string& header : m_headers)
        {
            ImGui::TableSetupColumn(header.data());
        }

        ImGui::TableHeadersRow();

        // clean items
        if (!m_itemsDirty.empty())
        {
            m_items.erase(std::remove_if(m_items.begin(), m_items.end(), [this](const StructItem& item)
            {
                if (auto it = std::find(m_itemsDirty.begin(), m_itemsDirty.end(), &item); it == m_itemsDirty.end())
                    return false;
                else
                {
                    m_itemsDirty.erase(it);

                    if (&item == m_lastPublishedItem)
                        m_lastPublishedItem = nullptr;

                    m_itemsStorage.erase(item.model().updateIndex());
                    return true;
                }
            }), m_items.end());

            m_itemsDirty.clear();
        }

        // sort items
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_containerChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_items.begin(), m_items.end(), [sortSpecs](const StructItem& lhs, const StructItem& rhs)
            {
                return lhs.less(*sortSpecs, rhs);
            });

            m_containerChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render items
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(m_items.size()));

        while (clipper.Step())
        {
            for (int itemIndex = clipper.DisplayStart; itemIndex < clipper.DisplayEnd; ++itemIndex)
            {
                // render items
                {
                    StructItem& item = m_items[itemIndex];
                    bool hoverCondition = ImGui::GetIO().KeyAlt;
                    item.render(hoverCondition);
                }

                // render quick info tooltip
                if (const StructItem& item = m_items[itemIndex]; item.isHovered())
                {
                    item.renderTooltip();

                    // open instance in struct edit when clicked
                    if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
                        editInstance = item.model().updatedInstanceModel().instance();
                }

                // context menu
                {
                    const StructItem& item = m_items[itemIndex];

                    if (ImGui::BeginPopupContextItem(item.widgetId()))
                    {
                        ImGuiExt::TextColored(m_structDescriptorModel->declarationText());
                        ImGui::Separator();

                        std::vector<std::reference_wrapper<const StructItem>> selection;
                        std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(selection), [](const StructItem& item)
                        {
                            return item.isSelected();
                        });

                        if (selection.size() <= 1 && ImGui::MenuItem(m_structDescriptorModel->descriptor().cached() ? "View/Update" : "View/Publish"))
                            editInstance = item.model().updatedInstanceModel().instance();

                        if (m_structDescriptorModel->descriptor().cached())
                        {
                            if (selection.size() <= 1 && ImGui::MenuItem("Remove [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                                dots::remove(item.model().updatedInstanceModel().instance());

                            if (selection.size() >= 2 && ImGui::MenuItem("Remove Selection [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                            {
                                for (const StructItem& selected : selection)
                                {
                                    dots::remove(selected.model().updatedInstanceModel().instance());
                                }
                            }
                        }

                        ImGui::EndPopup();
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    // struct edit
    {
        if (editInstance != std::nullopt)
            m_publishDialog.emplace(StructModel{ *m_structDescriptorModel, std::move(*editInstance) });
    }

    ImGui::TreePop();
}

void StructList::renderActivity()
{
    if (m_lastPublishedItem == nullptr)
        ImGui::TextUnformatted("      ");
    else
    {
        ImGuiExt::ColoredText text = m_lastPublishedItem->model().metadataModel().lastOperationText();
        constexpr float AnimationDuration = 1.0f;
        text.second.w = AnimationDuration - std::min(m_lastUpdateDelta, AnimationDuration) / AnimationDuration;
        ImGuiExt::TextColored(text);
    }
}

void StructList::renderActivityDot()
{
    if (m_lastPublishedItem == nullptr)
        ImGui::TextUnformatted("   ");
    else
    {
        ImVec4 color = m_lastPublishedItem->model().metadataModel().lastOperationText().second;
        constexpr float AnimationDuration = 1.0f;
        color.w = AnimationDuration - std::min(m_lastUpdateDelta, AnimationDuration) / AnimationDuration;

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Bullet();
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
    }
}
