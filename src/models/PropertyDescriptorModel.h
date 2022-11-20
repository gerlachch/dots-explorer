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
    const ImVec4& valueColor() const;

private:

    using property_path_ref_t = std::reference_wrapper<const dots::type::PropertyPath>;

    struct data
    {
        std::vector<ImGuiExt::ColoredText> declarationText;
        ImVec4 valueColor;
        property_path_ref_t propertyPath;
    };

    std::shared_ptr<data> m_data;
};
