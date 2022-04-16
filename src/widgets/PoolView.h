#pragma once
#include <vector>
#include <dots/dots.h>

struct PoolView
{
    PoolView();
    void render();

private:

    std::vector<dots::Subscription> m_subscriptions;
};
