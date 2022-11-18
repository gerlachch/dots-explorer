#include <models/StructModel.h>
#include <dots_ext/struct_ops.h>

StructModel::StructModel(const StructDescriptorModel& descriptorModel, dots::type::AnyStruct instance) :
    m_descriptorModel{ descriptorModel },
    m_instance{ std::move(instance) }
{
    m_propertyModels.reserve(descriptorModel.propertyDescriptorModels().size());

    for (const PropertyDescriptorModel& propertyDescriptorModel : descriptorModel.propertyDescriptorModels())
    {
        m_propertyModels.emplace_back(propertyDescriptorModel, m_instance);
    }
}

StructModel::StructModel(const StructDescriptorModel& descriptorModel, const dots::type::Struct& instance) :
    StructModel(descriptorModel, copy_all(instance))
{
    /* do nothing */
}

StructModel::StructModel(const StructDescriptorModel& descriptorModel) :
    StructModel(descriptorModel, descriptorModel.descriptor())
{
    /* do nothing */
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
    return m_instance;
}
