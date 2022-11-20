#include <models/StructModel.h>
#include <dots_ext/struct_ops.h>

StructModel::StructModel(StructDescriptorModel descriptorModel, dots::type::AnyStruct instance) :
    m_data(std::make_shared<data>(data{
        .descriptorModel = std::move(descriptorModel),
        .instance = std::move(instance)
    }))
{
    m_data->propertyModels.reserve(m_data->descriptorModel.propertyDescriptorModels().size());

    for (const PropertyDescriptorModel& propertyDescriptorModel : m_data->descriptorModel.propertyDescriptorModels())
    {
        m_data->propertyModels.emplace_back(propertyDescriptorModel, m_data->instance);
    }
}

StructModel::StructModel(StructDescriptorModel descriptorModel, const dots::type::Struct& instance) :
    StructModel(std::move(descriptorModel), copy_all(instance))
{
    /* do nothing */
}

StructModel::StructModel(StructDescriptorModel descriptorModel) :
    StructModel(descriptorModel, descriptorModel.descriptor())
{
    /* do nothing */
}

const StructDescriptorModel& StructModel::descriptorModel() const
{
    return m_data->descriptorModel;
}

const std::vector<PropertyModel>& StructModel::propertyModels() const
{
    return m_data->propertyModels;
}

std::vector<PropertyModel>& StructModel::propertyModels()
{
    return m_data->propertyModels;
}

const dots::type::Struct& StructModel::instance() const
{
    return m_data->instance;
}

dots::type::Struct& StructModel::instance()
{
    return m_data->instance;
}
