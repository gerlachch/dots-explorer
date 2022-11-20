#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructDescriptorModel.h>
#include <models/PropertyModel.h>

struct StructModel
{
    StructModel(StructDescriptorModel descriptorModel, dots::type::AnyStruct instance);
    StructModel(StructDescriptorModel descriptorModel, const dots::type::Struct& instance);
    StructModel(StructDescriptorModel descriptorModel);

    const StructDescriptorModel& descriptorModel() const;

    const std::vector<PropertyModel>& propertyModels() const;
    std::vector<PropertyModel>& propertyModels();

    const dots::type::Struct& instance() const;
    dots::type::Struct& instance();

private:

    struct data
    {
        StructDescriptorModel descriptorModel;
        dots::type::AnyStruct instance;
        std::vector<PropertyModel> propertyModels;
    };

    std::shared_ptr<data> m_data;
};
