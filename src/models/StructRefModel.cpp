#include <models/StructRefModel.h>
#include <cassert>
#include <fmt/format.h>
#include <common/Colors.h>

StructRefModel::StructRefModel(const StructDescriptorModel& descriptorModel, dots::type::Struct& instance) :
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

StructRefModel::StructRefModel(const StructDescriptorModel& descriptorModel, const dots::type::Struct& instance) :
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

const StructDescriptorModel& StructRefModel::descriptorModel() const
{
    return m_descriptorModel;
}

const std::vector<PropertyModel>& StructRefModel::propertyModels() const
{
    return m_propertyModels;
}

std::vector<PropertyModel>& StructRefModel::propertyModels()
{
    return m_propertyModels;
}

const dots::type::Struct& StructRefModel::instance() const
{
    return m_instance;
}

dots::type::Struct& StructRefModel::instance()
{
    assert(m_mutable);
    return m_instance;
}

void StructRefModel::fetch()
{
    for (PropertyModel& propertyModel : m_propertyModels)
    {
        propertyModel.fetch();
    }
}
