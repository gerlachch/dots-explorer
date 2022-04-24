#pragma once
#include <vector>
#include <imgui.h>
#include <dots/type/StructDescriptor.h>

struct StructDescription
{
    StructDescription(const dots::type::StructDescriptor<>& descriptor);
    void render() const;

private:

    using colored_text_t = std::pair<std::string, ImVec4>;
    std::vector<colored_text_t> m_parts;
};
