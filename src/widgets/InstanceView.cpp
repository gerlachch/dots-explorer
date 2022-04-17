#include <widgets/InstanceView.h>

InstanceView::InstanceView(const dots::type::Struct& instance) :
    m_instance{ instance }
{
    for (auto property : m_instance.get())
    {
        m_propertyViews.emplace_back(property);
    }

    update();
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

void InstanceView::update()
{
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
