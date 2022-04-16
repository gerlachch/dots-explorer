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

    void render();

private:

    bool m_poolChanged;
    std::vector<std::unique_ptr<ContainerView>> m_containerViews;
    dots::Subscription m_subscription;
};
