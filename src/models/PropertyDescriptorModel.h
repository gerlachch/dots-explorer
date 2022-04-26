#pragma once
#include <vector>
#include <functional>
#include <common/ImGuiExt.h>
#include <dots/type/PropertyPath.h>

struct PropertyDescriptorModel
{
    PropertyDescriptorModel(const dots::type::PropertyPath& propertyPath);

    const dots::type::PropertyPath& propertyPath() const;
    const std::vector<ImGuiExt::ColoredText>& declarationText() const;

    void fetch();
    void refresh();

private:

    using property_path_ref_t = std::reference_wrapper<const dots::type::PropertyPath>;
    
    std::vector<ImGuiExt::ColoredText> m_declarationText;
    property_path_ref_t m_propertyPath;
};
