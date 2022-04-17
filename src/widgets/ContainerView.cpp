#include <widgets/ContainerView.h>
#include <imgui.h>

ContainerView::ContainerView(const dots::type::StructDescriptor<>& descriptor) :
    m_container{ dots::container(descriptor) },
    m_subscription{ dots::subscribe(descriptor, [this](const dots::Event<>& event)
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
    }) }
{
    /* do nothing */
}

const dots::Container<>& ContainerView::container() const
{
    return m_container;
}

void ContainerView::render()
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
            std::sort(m_instanceViews.begin(), m_instanceViews.end(), [&descriptor, sortSpecs](const InstanceView& lhsView, const InstanceView& rhsView)
            {
                const dots::type::Struct& lhs = lhsView.instance();
                const dots::type::Struct& rhs = rhsView.instance();

                for (int i = 0; i < sortSpecs->SpecsCount; ++i)
                {
                    const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs->Specs[i];

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

                    const dots::type::PropertyDescriptor& propertyDescriptor = descriptor.propertyDescriptors()[sortSpec.ColumnIndex];
                    const dots::type::ProxyProperty<> lhsProperty{ const_cast<dots::type::Struct&>(lhs), propertyDescriptor };
                    const dots::type::ProxyProperty<> rhsProperty{ const_cast<dots::type::Struct&>(rhs), propertyDescriptor };

                    if (compare(lhsProperty, rhsProperty))
                    {
                        return true;
                    }
                    else if (compare(rhsProperty, lhsProperty))
                    {
                        return false;
                    }
                }

                return lhs._less(rhs, descriptor.keyProperties());
            });

            m_containerChanged = false;
            sortSpecs->SpecsDirty = false;
        }

        // render instance views
        for (InstanceView& instanceView : m_instanceViews)
        {
            instanceView.render();
        }

        ImGui::EndTable();
    }
}
