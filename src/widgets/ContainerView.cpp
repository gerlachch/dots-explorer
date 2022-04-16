#include <widgets/ContainerView.h>
#include <imgui.h>

ContainerView::ContainerView(const dots::type::StructDescriptor<>& descriptor) :
    m_container{ dots::container(descriptor) },
    m_subscription{ dots::subscribe(descriptor, [this](const dots::Event<>& event)
    {
        m_containerChanged = true;

        if (event.isCreate())
        {
            m_instances.emplace_back(event.updated());
        }
        else if (event.isRemove())
        {
            auto it = std::find_if(m_instances.begin(), m_instances.end(), [&event](const dots::type::Struct& instance)
            {
                return instance._same(event.updated());
            });

            if (it != m_instances.end())
            {
                m_instances.erase(it);
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
            std::sort(m_instances.begin(), m_instances.end(), [&descriptor, sortSpecs](const dots::type::Struct& lhs, const dots::type::Struct& rhs)
            {
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
        for (const dots::type::Struct& instance : m_instances)
        {
            for (auto property : instance)
            {
                ImGui::TableNextColumn();

                if (property.isValid())
                {
                    std::string value = dots::to_string(property);
                    
                    switch (property.descriptor().valueDescriptor().type())
                    {
                        case dots::type::Type::boolean:
                            ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::string:
                            ImGui::TextColored(ImVec4{ 0.91f, 0.79f, 0.73f, 1.0f }, "\"%s\"", value.data());
                            break;
                        case dots::type::Type::int8:
                        case dots::type::Type::uint8:
                        case dots::type::Type::int16:
                        case dots::type::Type::uint16:
                        case dots::type::Type::int32:
                        case dots::type::Type::uint32:
                        case dots::type::Type::int64:
                        case dots::type::Type::uint64:
                        case dots::type::Type::float32:
                        case dots::type::Type::float64:
                            ImGui::TextColored(ImVec4{ 0.72f, 0.84f, 0.64f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::Enum:
                            ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, value.data());
                            break;
                        case dots::type::Type::property_set:
                        case dots::type::Type::timepoint:
                        case dots::type::Type::steady_timepoint:
                        case dots::type::Type::duration:
                        case dots::type::Type::uuid:
                        case dots::type::Type::Vector:
                        case dots::type::Type::Struct:
                        default: 
                            ImGui::TextUnformatted(value.data());
                    }
                }
                else
                {
                    ImGui::TextDisabled("<invalid>");
                }
            }
        }

        ImGui::EndTable();
    }
}
