#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructDescriptorModel.h>
#include <models/PropertyModel.h>

struct StructRefModel
{
    StructRefModel(const StructDescriptorModel& descriptorModel, dots::type::Struct& instance);
    StructRefModel(const StructDescriptorModel& descriptorModel, const dots::type::Struct& instance);

    const StructDescriptorModel& descriptorModel() const;
    
    const std::vector<PropertyModel>& propertyModels() const;
    std::vector<PropertyModel>& propertyModels();

    const dots::type::Struct& instance() const;
    dots::type::Struct& instance();

    void fetch();

private:

    using descriptor_model_ref_t = std::reference_wrapper<const StructDescriptorModel>;
    using struct_ref_t = std::reference_wrapper<dots::type::Struct>;

    bool m_mutable;
    descriptor_model_ref_t m_descriptorModel;
    struct_ref_t m_instance;
    std::vector<PropertyModel> m_propertyModels;
};
