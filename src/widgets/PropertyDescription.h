#pragma once
#include <vector>
#include <imgui.h>
#include <dots/type/PropertyPath.h>

struct PropertyDescription
{
    PropertyDescription(const dots::type::PropertyPath& propertyPath);
    void render() const;

private:

    using colored_text_t = std::pair<std::string, ImVec4>;
    std::vector<colored_text_t> m_parts;
};
