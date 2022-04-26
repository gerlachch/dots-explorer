#include <models/StructModel.h>
#include <cassert>
#include <fmt/format.h>
#include <common/Colors.h>

StructModel::StructModel(const StructDescriptorModel& descriptorModel, dots::type::Struct& instance) :
    m_mutable(true),
    m_descriptorModel{ descriptorModel },
    m_instance{ instance }
{
    m_propertyModels.reserve(descriptorModel.propertyDescriptorModels().size());

    for (const PropertyDescriptorModel& propertyDescriptorModel : descriptorModel.propertyDescriptorModels())
    {
        m_propertyModels.emplace_back(propertyDescriptorModel, instance);
    }
}

StructModel::StructModel(const StructDescriptorModel& descriptorModel, const dots::type::Struct& instance) :
    m_mutable(false),
    m_descriptorModel{ descriptorModel },
    m_instance{ const_cast<dots::type::Struct&>(instance)}
{
    m_propertyModels.reserve(descriptorModel.propertyDescriptorModels().size());

    for (const PropertyDescriptorModel& propertyDescriptorModel : descriptorModel.propertyDescriptorModels())
    {
        m_propertyModels.emplace_back(propertyDescriptorModel, instance);
    }
}

const StructDescriptorModel& StructModel::descriptorModel() const
{
    return m_descriptorModel;
}

const std::vector<PropertyModel>& StructModel::propertyModels() const
{
    return m_propertyModels;
}

std::vector<PropertyModel>& StructModel::propertyModels()
{
    return m_propertyModels;
}

const dots::type::Struct& StructModel::instance() const
{
    return m_instance;
}

dots::type::Struct& StructModel::instance()
{
    assert(m_mutable);
    return m_instance;
}

void StructModel::fetch()
{
    for (PropertyModel& propertyModel : m_propertyModels)
    {
        propertyModel.fetch();
    }
}
