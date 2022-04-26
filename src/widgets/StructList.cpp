#include <widgets/StructList.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/format.h>
#include <common/ImGuiExt.h>
#include <widgets/StructView.h>
#include <DotsClearCache.dots.h>

StructList::StructList(const dots::type::StructDescriptor<>& descriptor) :
    m_lastPublishedRow(nullptr),
    m_lastPublishedRowTime{ dots::timepoint_t::min() },
    m_containerChanged(false),
    m_containerStorage{ descriptor.cached() ? std::optional<dots::Container<>>{ std::nullopt } : dots::Container<>{ descriptor } },
    m_container{ descriptor.cached() ? dots::container(descriptor) : *m_containerStorage },
    m_structDescriptorModel{ descriptor }
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
            case 1:  less = compare(container().size(), other.container().size()); break;
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

    auto [it, emplaced] = m_rowsStorage.try_emplace(instance, StructListRow{ m_structDescriptorModel, *instance });
    StructListRow& row = it->second;

    if (emplaced)
    {
        m_rows.emplace_back(row);
    }

    row.metadataModel().fetch(event);
    row.structModel().fetch();

    if (dots::timepoint_t lastPublished = row.metadataModel().lastPublished(); lastPublished > m_lastPublishedRowTime)
    {
        m_lastPublishedRow = &row;
        m_lastPublishedRowTime = lastPublished;
    }
}

bool StructList::renderBegin()
{
    bool containerOpen = ImGui::TreeNodeEx(container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);
    bool openStructEdit = false;

    // render quick info tooltip for last published struct instance
    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt && !m_rows.empty())
    {
        if (m_lastPublishedRow == nullptr)
        {
            m_lastPublishedRow = &std::max_element(m_rows.begin(), m_rows.end(), [this](const StructListRow& lhs, const StructListRow& rhs)
            {
                return lhs.metadataModel().lastPublished() < rhs.metadataModel().lastPublished();
            })->get();
            m_lastPublishedRowTime = m_lastPublishedRow->metadataModel().lastPublished();
        }

        ImGui::BeginTooltip();
        StructView structView{ m_lastPublishedRow->metadataModel(), m_lastPublishedRow->structModel() };
        structView.render();
        ImGui::EndTooltip();
    }

    // context menu
    {
        if (ImGui::BeginPopupContextItem())
        {
            ImGuiExt::TextColored(m_structDescriptorModel.declarationText());
            ImGui::Separator();

            if (ImGui::MenuItem("Create/Update"))
            {
                openStructEdit = true;
            }

            if (ImGui::MenuItem("Remove All", nullptr, false, ImGui::GetIO().KeyCtrl))
            {
                dots::publish(DotsClearCache{ 
                    DotsClearCache::typeNames_i{ dots::vector_t<dots::string_t>{ container().descriptor().name() } }
                });
            }

            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("Hold CTRL key to enable.");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

            ImGui::EndPopup();
        }
    }

    // struct edit
    {
        if (openStructEdit)
        {
            m_structEdit.emplace(m_structDescriptorModel, container().descriptor());
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

    if (ImGui::BeginTable(descriptor.name().data(), StructListRow::MetaData::MetaDataSize + static_cast<int>(m_headers.size()), TableFlags))
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

        // clean rows
        {
            m_rows.erase(std::remove_if(m_rows.begin(), m_rows.end(), [this](const StructListRow& row)
            {
                if (row.metadataModel().lastOperation() == DotsMt::remove)
                {
                    if (&row == m_lastPublishedRow)
                    {
                        m_lastPublishedRow = nullptr;
                    }

                    m_rowsStorage.erase(&row.structModel().instance());
                    return true;
                }
                else
                {
                    return false;
                }
            }), m_rows.end());
        }

        // sort rows
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_containerChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_rows.begin(), m_rows.end(), [sortSpecs](const StructListRow& lhs, const StructListRow& rhs)
            {
                return lhs.less(*sortSpecs, rhs);
            });

            m_containerChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render rows
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(m_rows.size()));

        while (clipper.Step())
        {
            for (int rowIndex = clipper.DisplayStart; rowIndex < clipper.DisplayEnd; ++rowIndex)
            {
                {
                    StructListRow& row = m_rows[rowIndex];
                    row.render();

                    // render quick info tooltip
                    if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
                    {
                        ImGui::BeginTooltip();
                        StructView structView{ row.metadataModel(), row.structModel() };
                        structView.render();
                        ImGui::EndTooltip();
                    }
                }

                // context menu
                {
                    const StructListRow& row = m_rows[rowIndex];

                    if (ImGui::BeginPopupContextItem(row.widgetId()))
                    {
                        ImGuiExt::TextColored(m_structDescriptorModel.declarationText());
                        ImGui::Separator();

                        std::vector<std::reference_wrapper<const StructListRow>> selection;
                        std::copy_if(m_rows.begin(), m_rows.end(), std::back_inserter(selection), [](const StructListRow& row)
                        {
                            return row.isSelected();
                        });

                        if (selection.empty() && ImGui::MenuItem("View/Update"))
                        {
                            editInstance = row.structModel().instance();
                        }

                        if (selection.empty() && ImGui::MenuItem("Remove", nullptr, false, ImGui::GetIO().KeyCtrl))
                        {
                            dots::remove(row.structModel().instance());
                        }

                        if (!selection.empty() && ImGui::MenuItem("Remove Selection", nullptr, false, ImGui::GetIO().KeyCtrl))
                        {
                            for (const StructListRow& selected : selection)
                            {
                                dots::remove(selected.structModel().instance());
                            }
                        }

                        ImGui::SameLine();
                        ImGui::TextDisabled("(?)");
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::TextUnformatted("Hold CTRL key to enable.");
                            ImGui::PopTextWrapPos();
                            ImGui::EndTooltip();
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
