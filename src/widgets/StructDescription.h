#pragma once
#include <vector>
#include <imgui.h>
#include <dots/type/StructDescriptor.h>

struct StructDescription
{
    StructDescription(const dots::type::StructDescriptor<>& descriptor);

    const dots::type::StructDescriptor<>& descriptor() const;
    void render() const;

private:

    using struct_descriptor_ref = std::reference_wrapper<const dots::type::StructDescriptor<>>;
    using colored_text_t = std::pair<std::string, ImVec4>;

    std::vector<colored_text_t> m_parts;
    struct_descriptor_ref m_descriptor;
};
