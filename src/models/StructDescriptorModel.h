#pragma once
#include <vector>
#include <dots/type/StructDescriptor.h>
#include <common/ImGuiExt.h>
#include <models/PropertyDescriptorModel.h>

struct StructDescriptorModel
{
    StructDescriptorModel(const dots::type::StructDescriptor<>& descriptor);

    const std::vector<PropertyDescriptorModel>& propertyDescriptorModels() const;

    const dots::type::StructDescriptor<>& descriptor() const;
    const std::vector<ImGuiExt::ColoredText>& declarationText() const;

    void fetch();
    void refresh();

private:

    using descriptor_ref_t = std::reference_wrapper<const dots::type::StructDescriptor<>>;
    using colored_text_t = std::pair<std::string, ImVec4>;

    std::vector<colored_text_t> m_declarationText;
    std::vector<PropertyDescriptorModel> m_propertyDescriptorModels;
    descriptor_ref_t m_descriptor;
};
