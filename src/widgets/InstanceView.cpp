#include <widgets/InstanceView.h>
#include <fmt/format.h>

InstanceView::InstanceView(const dots::type::Struct& instance) :
    m_id{ fmt::format("InstanceView-{}_ContextMenu", ++M_id) },
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

    // context menu
    bool openInstanceEdit = false;

    {
        if (ImGui::BeginPopupContextItem(m_id.data()))
        {
            ImGui::TextColored(ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f }, "struct");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f }, "%s", m_instance.get()._descriptor().name().data());
            ImGui::Separator();
            ImGui::MenuItem("View/Update", nullptr, &openInstanceEdit);

            ImGui::EndPopup();
        }
    }

    // instance edit
    {
        if (openInstanceEdit)
        {
            m_instanceEdit.emplace(m_instance.get());
        }

        if (m_instanceEdit != std::nullopt && !m_instanceEdit->render())
        {
            m_instanceEdit = std::nullopt;
        }
    }
}
