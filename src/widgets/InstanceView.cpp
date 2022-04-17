#include <widgets/InstanceView.h>

InstanceView::InstanceView(const dots::type::Struct& instance) :
    m_instance{ instance }
{
    update();
}

const dots::type::Struct& InstanceView::instance() const
{
    return m_instance;
}

void InstanceView::update()
{
    m_propertyViews.clear();

    for (auto property : m_instance.get())
    {
        std::string value = dots::to_string(property);

        if (property.isValid())
        {
            switch (property.descriptor().valueDescriptor().type())
            {
                case dots::type::Type::boolean:
                m_propertyViews.emplace_back(property_view{ std::move(value), ImVec4{ 0.34f, 0.61f, 0.84f, 1.0f } });
                    break;
                case dots::type::Type::string:
                    m_propertyViews.emplace_back(property_view{ "\"" + value +"\"", ImVec4{0.91f, 0.79f, 0.73f, 1.0f } });
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
                    m_propertyViews.emplace_back(property_view{ std::move(value), ImVec4{ 0.72f, 0.84f, 0.64f, 1.0f } });
                    break;
                case dots::type::Type::Enum:
                    m_propertyViews.emplace_back(property_view{ std::move(value), ImVec4{ 0.31f, 0.79f, 0.69f, 1.0f } });
                    break;
                case dots::type::Type::property_set:
                case dots::type::Type::timepoint:
                case dots::type::Type::steady_timepoint:
                case dots::type::Type::duration:
                case dots::type::Type::uuid:
                case dots::type::Type::Vector:
                case dots::type::Type::Struct:
                default: 
                    m_propertyViews.emplace_back(property_view{ std::move(value) });
            }
        }
        else
        {
            m_propertyViews.emplace_back(property_view{ std::move(value), std::nullopt, false });
        }
    }
}

void InstanceView::render()
{
    for (const property_view& propertyView : m_propertyViews)
    {
        ImGui::TableNextColumn();

        if (propertyView.isValid)
        {
            if (propertyView.color == std::nullopt)
            {
                ImGui::TextUnformatted(propertyView.value.data());
            }
            else
            {
                ImGui::TextColored(*propertyView.color, propertyView.value.data());
            }
        }
        else
        {
            ImGui::TextDisabled(propertyView.value.data());
        }
    }
}
