#pragma once
#include <dots/dots.h>

struct ContainerView
{
    ContainerView(const dots::type::StructDescriptor<>& descriptor);
    void render();

private:

    std::reference_wrapper<const dots::type::StructDescriptor<>> m_descriptor;
};
