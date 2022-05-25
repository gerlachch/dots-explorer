#include <models/PropertyModel.h>
#include <cassert>
#include <common/Colors.h>

static dots::serialization::StringSerializer StringSerializer;

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

const std::string& PropertyModel::toString() const
{
    (void)valueText();
    return m_valueStr;
}

bool PropertyModel::fromString(const std::string& value)
{
    try
    {
        dots::from_string(value, m_property);
        fetch();

        return true;
    }
    catch (...)
    {
        return false;
    }
}

void PropertyModel::invalidate()
{
    m_property.destroy();
    fetch();
}

void PropertyModel::randomize()
{
    M_randomizer.randomize(m_property);
    fetch();
}

bool PropertyModel::valueChanged() const
{
    return m_valueStr.empty();
}

ImGuiExt::ColoredTextView PropertyModel::valueText() const
{
    if (m_valueStr.empty())
    {
        dots::type::Type type = descriptorModel().propertyPath().destination().valueDescriptor().type();

        StringSerializer.output().clear();
        StringSerializer.serialize(m_property);

        if (m_property.isValid() && type == dots::type::Type::string)
        {
            m_valueStr += '"';
            m_valueStr += StringSerializer.output();
            m_valueStr += '"';
        }
        else
        {
            m_valueStr = StringSerializer.output();
        }
    }

    if (m_property.isValid())
    {
        return { m_valueStr, descriptorModel().valueColor() };
    }
    else
    {
        return { m_valueStr, ColorThemeActive.Disabled };
    }
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
    m_valueStr.clear();
}
