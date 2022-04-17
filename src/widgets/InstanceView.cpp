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
