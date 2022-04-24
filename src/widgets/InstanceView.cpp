#include <widgets/InstanceView.h>
#include <imgui_internal.h>
#include <fmt/format.h>

InstanceView::InstanceView(const dots::type::Struct& instance) :
    m_widgetId{ fmt::format("InstanceView-{}", M_nextWidgetId++) },
    m_lastOperation(DotsMt::create),
    m_instance{ instance }
{
    const auto& propertyPaths = m_instance.get()._descriptor().propertyPaths();

    if (propertyPaths.size() <= IMGUI_TABLE_MAX_COLUMNS)
    {
        for (const dots::type::PropertyPath& propertyPath : propertyPaths)
        {
            const dots::type::PropertyDescriptor& propertyDescriptor = propertyPath.destination();

            if (propertyDescriptor.valueDescriptor().type() == dots::type::Type::Struct)
            {
                continue;
            }

            if (propertyPath.elements().size() == 1)
            {
                m_propertyViews.emplace_back(dots::type::ProxyProperty<>{ const_cast<dots::type::Struct&>(m_instance.get()), propertyDescriptor });
            }
            else
            {
                m_propertyViews.emplace_back(dots::type::ProxyProperty<>{ const_cast<dots::type::Struct&>(m_instance.get()), propertyPath });
            }
        }
    }
    else
    {
        for (auto property : m_instance.get())
        {
            m_propertyViews.emplace_back(property);
        }
    }
}

const char* InstanceView::widgetId() const
{
    return m_widgetId.data();
}

DotsMt InstanceView::lastOperation() const
{
    return m_lastOperation;
}

const dots::type::Struct& InstanceView::instance() const
{
    return m_instance;
}

bool InstanceView::less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const
{
    for (int i = 0; i < sortSpecs.SpecsCount; ++i)
    {
        const ImGuiTableColumnSortSpecs& sortSpec = sortSpecs.Specs[i];
        const PropertyView& propertyViewThis = m_propertyViews[sortSpec.ColumnIndex];
        const PropertyView& propertyViewOther = other.m_propertyViews[sortSpec.ColumnIndex];

        if (propertyViewThis.less(sortSpec, propertyViewOther))
        {
            return true;
        }
        else if (propertyViewOther.less(sortSpec, propertyViewThis))
        {
            return false;
        }
    }

    return instance()._less(other.instance(), instance()._keyProperties());
}

bool InstanceView::isSelected() const
{
    return std::any_of(m_propertyViews.begin(), m_propertyViews.end(), [](const PropertyView& propertyView){ return propertyView.isSelected(); });
}

void InstanceView::update(const dots::Event<>& event)
{
    m_lastOperation = event.mt();

    for (PropertyView& propertyView : m_propertyViews)
    {
        propertyView.update();
    }
}

void InstanceView::render()
{
    for (PropertyView& propertyView : m_propertyViews)
    {
        ImGui::TableNextColumn();
        propertyView.render();
    }
}
