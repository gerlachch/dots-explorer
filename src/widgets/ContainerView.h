#pragma once
#include <dots/dots.h>
#include <widgets/InstanceView.h>

struct ImGuiTableSortSpecs;

struct ContainerView
{
    ContainerView(const dots::type::StructDescriptor<>& descriptor);
    ContainerView(const ContainerView& other) = delete;
    ContainerView(ContainerView&& other) = delete;
    ~ContainerView() = default;

    ContainerView& operator = (const ContainerView& rhs) = delete;
    ContainerView& operator = (ContainerView&& rhs) = delete;

    const dots::Container<>& container() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const ContainerView& other) const;
    void render();

private:

    using container_ref_t = std::reference_wrapper<const dots::Container<>>;

    std::vector<InstanceView> m_instanceViews;
    bool m_containerChanged;
    container_ref_t m_container;
    dots::Subscription m_subscription;
};
