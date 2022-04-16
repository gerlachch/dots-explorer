#pragma once
#include <vector>
#include <dots/dots.h>
#include <widgets/ContainerView.h>

struct PoolView
{
    PoolView();
    void render();

private:

    bool m_poolChanged;
    std::vector<ContainerView> m_containerViews;
    dots::Subscription m_subscription;
};
