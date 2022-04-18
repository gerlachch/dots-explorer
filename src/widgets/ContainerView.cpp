#include <widgets/ContainerView.h>
#include <imgui.h>
#include <fmt/format.h>

ContainerView::ContainerView(const dots::type::StructDescriptor<>& descriptor) :
    m_containerChanged(false),
    m_container{ dots::container(descriptor) },
    m_subscription{ dots::subscribe(descriptor, { &ContainerView::update, this }) }
{
    /* do nothing */
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
            ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, "struct");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, "%s", container().descriptor().name().data());
            ImGui::Separator();

            if (ImGui::MenuItem("Create/Update"))
            {
                openInstanceEdit = true;
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
        ImGuiTableFlags_SortMulti
    ;

    const dots::type::StructDescriptor<>& descriptor = m_container.get().descriptor();
    std::optional<dots::type::AnyStruct> editInstance;

    if (ImGui::BeginTable(descriptor.name().data(), static_cast<int>(descriptor.propertyDescriptors().size()), TableFlags))
    {
        // create headers
        for (const dots::type::PropertyDescriptor& propertyDescriptor : descriptor.propertyDescriptors())
        {
            ImGui::TableSetupColumn(propertyDescriptor.name().data());
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
                    ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, "struct");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, "%s", instanceView.instance()._descriptor().name().data());

                    ImGui::Separator();

                    if (ImGui::MenuItem("View/Update"))
                    {
                        editInstance = instanceView.instance();
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
