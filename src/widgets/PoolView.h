#pragma once
#include <array>
#include <vector>
#include <optional>
#include <dots/dots.h>
#include <widgets/ContainerView.h>
#include <widgets/InstanceEdit.h>

struct PoolView
{
    PoolView();
    PoolView(const PoolView& other) = delete;
    PoolView(PoolView&& other) = delete;
    ~PoolView() = default;

    PoolView& operator = (const PoolView& rhs) = delete;
    PoolView& operator = (PoolView&& rhs) = delete;

    void update(const dots::type::StructDescriptor<>& descriptor);
    void render();

private:

    std::optional<InstanceEdit> m_instanceEdit;
    std::vector<std::shared_ptr<ContainerView>> m_containerViews;
    std::vector<std::shared_ptr<ContainerView>> m_containerViewsFiltered;
    std::array<char, 64> m_containerFilter;
    bool m_containerFilterChanged;
    bool m_poolChanged;
    dots::Subscription m_subscription;
};
