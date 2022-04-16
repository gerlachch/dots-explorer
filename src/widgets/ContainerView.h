#pragma once
#include <dots/dots.h>

struct ContainerView
{
    ContainerView(const dots::type::StructDescriptor<>& descriptor);
    const dots::Container<>& container() const;
    void render();

private:

    std::reference_wrapper<const dots::Container<>> m_container;
    dots::Subscription m_subscription;
};
