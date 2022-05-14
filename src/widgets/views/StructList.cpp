#include <widgets/views/StructList.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/ImGuiExt.h>
#include <widgets/views/StructView.h>
#include <DotsClearCache.dots.h>

StructList::StructList(const dots::type::StructDescriptor<>& descriptor, const PublisherModel& publisherModel) :
    m_lastPublishedItem(nullptr),
    m_lastPublishedItemTime{ dots::timepoint_t::min() },
    m_lastUpdateDelta(0.0f),
    m_containerChanged(false),
    m_containerStorage{ descriptor.cached() ? std::optional<dots::Container<>>{ std::nullopt } : dots::Container<>{ descriptor } },
    m_container{ descriptor.cached() ? dots::container(descriptor) : *m_containerStorage },
    m_structDescriptorModel{ descriptor },
    m_publisherModel{ publisherModel }
{
    const auto& propertyPaths = descriptor.propertyPaths();

    if (propertyPaths.size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (const dots::type::PropertyPath& propertyPath : propertyPaths)
        {
            const dots::type::PropertyDescriptor& propertyDescriptor = propertyPath.destination();

            if (propertyDescriptor.valueDescriptor().type() == dots::type::Type::Struct)
            {
                continue;
            }

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

const dots::Container<>& StructList::container() const
{
    return m_container;
}

bool StructList::less(const ImGuiTableSortSpecs& sortSpecs, const StructList& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];

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
            case 0:  less = compare(container().descriptor().name(), other.container().descriptor().name()); break;
            case 1:  less = compare(m_lastUpdateDelta, other.m_lastUpdateDelta); break;
            case 2:  less = compare(m_lastUpdateDelta, other.m_lastUpdateDelta); break;
            case 3:  less = compare(container().size(), other.container().size()); break;
            default: IM_ASSERT(0); break;
        }

        if (less)
        {
            return true;
        }
    }

    return false;
}

void StructList::update(const dots::Event<>& event)
{
    const dots::type::Struct* instance;

    if (container().descriptor().cached())
    {
        instance = &event.updated();
    }
    else
    {
        const auto& [updated, cloneInfo] = m_containerStorage->insert(event.header(), event.updated());
        instance = &*updated;
    }

    m_containerChanged = true;

    auto [it, emplaced] = m_itemsStorage.try_emplace(instance, StructItem{ m_structDescriptorModel, m_publisherModel, *instance });
    StructItem& item = it->second;

    if (emplaced)
    {
        m_items.emplace_back(item);
    }

    item.metadataModel().fetch(event);
    item.structModel().fetch();

    if (dots::timepoint_t lastPublished = item.metadataModel().lastPublished(); lastPublished > m_lastPublishedItemTime)
    {
        m_lastPublishedItem = &item;
        m_lastPublishedItemTime = lastPublished;
    }

    m_lastUpdateDelta = 0.0f;
}

bool StructList::renderBegin()
{
    m_lastUpdateDelta += ImGui::GetIO().DeltaTime;

    bool containerOpen = ImGui::TreeNodeEx(container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);
    bool openStructEdit = false;
    std::optional<dots::type::AnyStruct> editInstance;

    // render quick info tooltip for last published struct instance
    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt && !m_items.empty())
    {
        if (m_lastPublishedItem == nullptr)
        {
            m_lastPublishedItem = &std::max_element(m_items.begin(), m_items.end(), [this](const StructItem& lhs, const StructItem& rhs)
            {
                return lhs.metadataModel().lastPublished() < rhs.metadataModel().lastPublished();
            })->get();
            m_lastPublishedItemTime = m_lastPublishedItem->metadataModel().lastPublished();
        }

        ImGui::BeginTooltip();
        StructView structView{ m_lastPublishedItem->metadataModel(), m_lastPublishedItem->structModel() };
        structView.render();
        ImGui::EndTooltip();

        // open last published instance in struct edit when clicked
        if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
        {
            openStructEdit = true;
            editInstance = m_lastPublishedItem->structModel().instance();
        }
    }

    // context menu
    {
        if (ImGui::BeginPopupContextItem())
        {
            ImGuiExt::TextColored(m_structDescriptorModel.declarationText());
            ImGui::Separator();

            if (ImGui::MenuItem(m_structDescriptorModel.descriptor().cached() ? "Create/Update" : "Publish"))
            {
                openStructEdit = true;
            }

            if (m_structDescriptorModel.descriptor().cached())
            {
                if (ImGui::MenuItem("Remove All [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                {
                    dots::publish(DotsClearCache{ 
                        DotsClearCache::typeNames_i{ dots::vector_t<dots::string_t>{ container().descriptor().name() } }
                    });
                }
            }

            ImGui::EndPopup();
        }
    }

    // struct edit
    {
        if (openStructEdit)
        {
            if (editInstance == std::nullopt)
            {
                m_structEdit.emplace(m_structDescriptorModel, container().descriptor());
            }
            else
            {
                m_structEdit.emplace(m_structDescriptorModel, std::move(*editInstance));
            }
        }

        if (m_structEdit != std::nullopt && !m_structEdit->render())
        {
            m_structEdit = std::nullopt;
        }
    }

    return containerOpen;
}

void StructList::renderEnd()
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
        ImGuiTableFlags_Resizable      |
        ImGuiTableFlags_Sortable       |
        ImGuiTableFlags_SortMulti      |
        ImGuiTableFlags_Hideable
    ;

    const dots::type::StructDescriptor<>& descriptor = m_container.get().descriptor();
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
        {
            m_items.erase(std::remove_if(m_items.begin(), m_items.end(), [this](const StructItem& item)
            {
                if (item.metadataModel().lastOperation() == DotsMt::remove)
                {
                    if (&item == m_lastPublishedItem)
                    {
                        m_lastPublishedItem = nullptr;
                    }

                    m_itemsStorage.erase(&item.structModel().instance());
                    return true;
                }
                else
                {
                    return false;
                }
            }), m_items.end());
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
                    ImGui::BeginTooltip();
                    StructView structView{ item.metadataModel(), item.structModel() };
                    structView.render();
                    ImGui::EndTooltip();

                    // open instance in struct edit when clicked
                    if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_Left])
                    {
                        editInstance = item.structModel().instance();
                    }
                }

                // context menu
                {
                    const StructItem& item = m_items[itemIndex];

                    if (ImGui::BeginPopupContextItem(item.widgetId()))
                    {
                        ImGuiExt::TextColored(m_structDescriptorModel.declarationText());
                        ImGui::Separator();

                        std::vector<std::reference_wrapper<const StructItem>> selection;
                        std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(selection), [](const StructItem& item)
                        {
                            return item.isSelected();
                        });

                        if (selection.empty() && ImGui::MenuItem(m_structDescriptorModel.descriptor().cached() ? "View/Update" : "View/Publish"))
                        {
                            editInstance = item.structModel().instance();
                        }

                        if (m_structDescriptorModel.descriptor().cached())
                        {
                            if (selection.empty() && ImGui::MenuItem("Remove [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                            {
                                dots::remove(item.structModel().instance());
                            }

                            if (!selection.empty() && ImGui::MenuItem("Remove Selection [Hold CTRL]", nullptr, false, ImGui::GetIO().KeyCtrl))
                            {
                                for (const StructItem& selected : selection)
                                {
                                    dots::remove(selected.structModel().instance());
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
        {
            m_structEdit.emplace(m_structDescriptorModel, std::move(*editInstance));
        }
    }

    ImGui::TreePop();
}

void StructList::renderActivity()
{
    if (m_lastPublishedItem == nullptr)
    {
        ImGui::TextUnformatted("      ");
    }
    else
    {
        ImGuiExt::ColoredText text = m_lastPublishedItem->metadataModel().lastOperationText();
        constexpr float AnimationDuration = 1.0f;
        text.second.w = AnimationDuration - std::min(m_lastUpdateDelta, AnimationDuration) / AnimationDuration;
        ImGuiExt::TextColored(text);
    }
}

void StructList::renderActivityDot()
{
    if (m_lastPublishedItem == nullptr)
    {
        ImGui::TextUnformatted("   ");
    }
    else
    {
        ImVec4 color = m_lastPublishedItem->metadataModel().lastOperationText().second;
        constexpr float AnimationDuration = 1.0f;
        color.w = AnimationDuration - std::min(m_lastUpdateDelta, AnimationDuration) / AnimationDuration;

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Bullet();
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
    }
}
