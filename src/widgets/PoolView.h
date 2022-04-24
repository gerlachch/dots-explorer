#pragma once
#include <vector>
#include <dots/dots.h>
#include <widgets/ContainerView.h>

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

    std::vector<std::shared_ptr<ContainerView>> m_containerViews;
    std::vector<std::shared_ptr<ContainerView>> m_containerViewsFiltered;
    std::string m_containerFilterBuffer;
    bool m_poolChanged;
    bool m_showInternal;
    bool m_showEmpty;
    std::vector<dots::Subscription> m_subscriptions;
};
