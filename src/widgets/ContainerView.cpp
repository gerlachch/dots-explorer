#include <widgets/ContainerView.h>
#include <imgui.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <DotsClearCache.dots.h>

ContainerView::ContainerView(const dots::type::StructDescriptor<>& descriptor) :
    m_containerChanged(false),
    m_container{ dots::container(descriptor) }
{
    for (const dots::type::PropertyDescriptor& propertyDescriptor : descriptor.propertyDescriptors())
    {
        const std::string& name = propertyDescriptor.name();
        m_headers.emplace_back(propertyDescriptor.isKey() ? fmt::format("{} [key]", name) : name);
    }
}

const dots::Container<>& ContainerView::container() const
{
    return m_container;
}

bool ContainerView::less(const ImGuiTableSortSpecs& sortSpecs, const ContainerView& other) const
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

void ContainerView::update(const dots::Event<>& event)
{
    m_containerChanged = true;

    if (event.isCreate())
    {
        m_instanceViews.emplace_back(event.updated());
    }
    else
    {
        auto it = std::find_if(m_instanceViews.begin(), m_instanceViews.end(), [&event](const InstanceView& instanceView)
        {
            return instanceView.instance()._same(event.updated());
        });

        if (event.isUpdate())
        {
            it->update();
        }
        else/* if (event.isRemove())*/
        {
            m_instanceViews.erase(it);
        }
    }
}

bool ContainerView::renderBegin()
{
    bool containerOpen = ImGui::TreeNodeEx(container().descriptor().name().data(), ImGuiTreeNodeFlags_SpanFullWidth);
    bool openInstanceEdit = false;

    // context menu
    {
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::TextColored(ColorThemeActive.Keyword, "struct");
            ImGui::SameLine();
            ImGui::TextColored(ColorThemeActive.UserType, "%s", container().descriptor().name().data());
            ImGui::Separator();

            if (ImGui::MenuItem("Create/Update"))
            {
                openInstanceEdit = true;
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

    // instance edit
    {
        if (openInstanceEdit)
        {
            m_instanceEdit.emplace(container().descriptor());
        }

        if (m_instanceEdit != std::nullopt && !m_instanceEdit->render())
        {
            m_instanceEdit = std::nullopt;
        }
    }

    return containerOpen;
}

void ContainerView::renderEnd()
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

    if (ImGui::BeginTable(descriptor.name().data(), static_cast<int>(descriptor.propertyDescriptors().size()), TableFlags))
    {
        // create headers
        for (const std::string& header : m_headers)
        {
            ImGui::TableSetupColumn(header.data());
        }

        ImGui::TableHeadersRow();

        // sort instance views
        if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs(); m_containerChanged || sortSpecs->SpecsDirty)
        {
            std::sort(m_instanceViews.begin(), m_instanceViews.end(), [sortSpecs](const InstanceView& lhs, const InstanceView& rhs)
            {
                return lhs.less(*sortSpecs, rhs);
            });

            m_containerChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render instance views
        for (InstanceView& instanceView : m_instanceViews)
        {
            instanceView.render();

            // context menu
            {
                if (ImGui::BeginPopupContextItem(instanceView.widgetId()))
                {
                    ImGui::TextColored(ColorThemeActive.Keyword, "struct");
                    ImGui::SameLine();
                    ImGui::TextColored(ColorThemeActive.UserType, "%s", instanceView.instance()._descriptor().name().data());

                    ImGui::Separator();

                    std::vector<std::reference_wrapper<const InstanceView>> selection;
                    std::copy_if(m_instanceViews.begin(), m_instanceViews.end(), std::back_inserter(selection), [](const InstanceView& instanceView)
                    {
                        return instanceView.isSelected();
                    });

                    if (selection.empty() && ImGui::MenuItem("View/Update"))
                    {
                        editInstance = instanceView.instance();
                    }

                    if (selection.empty() && ImGui::MenuItem("Remove", nullptr, false, ImGui::GetIO().KeyCtrl))
                    {
                        dots::remove(instanceView.instance());
                    }

                    if (!selection.empty() && ImGui::MenuItem("Remove Selection", nullptr, false, ImGui::GetIO().KeyCtrl))
                    {
                        for (const InstanceView& selected : selection)
                        {
                            dots::remove(selected.instance());
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

        ImGui::EndTable();
    }

    // instance edit
    {
        if (editInstance != std::nullopt)
        {
            m_instanceEdit.emplace(std::move(*editInstance));
        }
    }

    ImGui::TreePop();
}
