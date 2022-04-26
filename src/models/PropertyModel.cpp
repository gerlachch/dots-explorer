#include <models/PropertyModel.h>
#include <cassert>
#include <common/Colors.h>

PropertyModel::PropertyModel(const PropertyDescriptorModel& descriptorModel, dots::type::Struct& instance) :
    m_mutable(true),
    m_descriptorModel{ descriptorModel },
    m_property{ instance, descriptorModel.propertyPath() }
{
    /* do nothing */
}

PropertyModel::PropertyModel(const PropertyDescriptorModel& descriptorModel, const dots::type::Struct& instance) :
    PropertyModel(descriptorModel, const_cast<dots::type::Struct&>(instance))
{
    m_mutable = false;
}

const PropertyDescriptorModel& PropertyModel::descriptorModel() const
{
    return m_descriptorModel;
}

const dots::type::ProxyProperty<>& PropertyModel::property() const
{
    return m_property;
}

dots::type::ProxyProperty<>& PropertyModel::property()
{
    assert(m_mutable);
    return m_property;
}

const ImGuiExt::ColoredText& PropertyModel::valueText() const
{
    auto& [text, color] = m_valueText;

    if (text.empty())
    {
        dots::type::Type type = descriptorModel().propertyPath().destination().valueDescriptor().type();

        if (m_property.isValid())
        {
            switch (type)
            {
                case dots::type::Type::boolean:
                    color = ColorThemeActive.Keyword;
                    break;
                case dots::type::Type::string:
                    color = ColorThemeActive.StringType;
                    break;
                case dots::type::Type::Enum:
                    color = ColorThemeActive.EnumType;
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
                case dots::type::Type::property_set:
                case dots::type::Type::timepoint:
                case dots::type::Type::steady_timepoint:
                case dots::type::Type::duration:
                case dots::type::Type::uuid:
                case dots::type::Type::Vector:
                case dots::type::Type::Struct:
                default:
                    color = ColorThemeActive.IntegralType;
                    break;
            }
        }
        else
        {
            color = ColorThemeActive.Disabled;
        }

        if (type == dots::type::Type::string)
        {
            text += '"';
            text += dots::to_string(m_property);
            text += '"';
        }
        else
        {
            text = dots::to_string(m_property);
        }
    }

    return m_valueText;
}

bool PropertyModel::less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyModel& other) const
{
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

    return compare(m_property, other.m_property);
}

void PropertyModel::fetch()
{
    auto& [text, color] = m_valueText;
    text.clear();
}
