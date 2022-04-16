#pragma once
#include <dots/dots.h>

struct ContainerView
{
    ContainerView(const dots::type::StructDescriptor<>& descriptor);
    const dots::Container<>& container() const;
    void render();

private:

    using struct_ref_t = std::reference_wrapper<const dots::type::Struct>;
    using container_ref_t = std::reference_wrapper<const dots::Container<>>;

    std::vector<struct_ref_t> m_instances;
    bool m_containerChanged;
    container_ref_t m_container;
    dots::Subscription m_subscription;
};
