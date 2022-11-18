#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructDescriptorModel.h>
#include <models/PropertyModel.h>

struct StructModel
{
    StructModel(const StructDescriptorModel& descriptorModel, dots::type::AnyStruct instance);
    StructModel(const StructDescriptorModel& descriptorModel, const dots::type::Struct& instance);
    StructModel(const StructDescriptorModel& descriptorModel);
    StructModel(const StructModel& other) = delete;
    StructModel(StructModel&& other) = default;
    ~StructModel() = default;

    StructModel& operator = (const StructModel& rhs) = delete;
    StructModel& operator = (StructModel&& rhs) = default;

    const StructDescriptorModel& descriptorModel() const;

    const std::vector<PropertyModel>& propertyModels() const;
    std::vector<PropertyModel>& propertyModels();

    const dots::type::Struct& instance() const;
    dots::type::Struct& instance();

private:

    using descriptor_model_ref_t = std::reference_wrapper<const StructDescriptorModel>;

    descriptor_model_ref_t m_descriptorModel;
    dots::type::AnyStruct m_instance;
    std::vector<PropertyModel> m_propertyModels;
};
