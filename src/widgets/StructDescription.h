#pragma once
#include <vector>
#include <common/ImGuiExt.h>
#include <dots/type/StructDescriptor.h>

struct StructDescription
{
    StructDescription(const dots::type::StructDescriptor<>& descriptor);

    const dots::type::StructDescriptor<>& descriptor() const;
    void render() const;

private:

    using struct_descriptor_ref = std::reference_wrapper<const dots::type::StructDescriptor<>>;

    std::vector<ImGuiExt::ColoredText> m_parts;
    struct_descriptor_ref m_descriptor;
};
