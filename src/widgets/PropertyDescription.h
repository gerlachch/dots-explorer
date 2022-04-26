#pragma once
#include <vector>
#include <functional>
#include <common/ImGuiExt.h>
#include <dots/type/PropertyPath.h>

struct PropertyDescription
{
    PropertyDescription(const dots::type::PropertyPath& propertyPath);

    const dots::type::PropertyPath& propertyPath() const;

    const ImVec4& valueColor() const;
    bool valueQuoted() const;

    void render() const;

private:

    using property_path_ref = std::reference_wrapper<const dots::type::PropertyPath>;

    std::vector<ImGuiExt::ColoredText> m_parts;
    ImVec4 m_valueColor;
    bool m_valueQuoted;
    property_path_ref m_propertyPath;
};
