#pragma once
#include <map>
#include <common/ImGuiExt.h>
#include <dots/dots.h>
#include <DotsClient.dots.h>

struct PublisherModel
{
    PublisherModel();

    const ImGuiExt::ColoredText& publisherNameText(uint32_t id) const;

private:

    ImGuiExt::ColoredText& publisherNameText(uint32_t id);

    void handleDotsClient(const dots::Event<DotsClient>& event);

    mutable std::map<uint32_t, ImGuiExt::ColoredText> m_publisherNameTexts;
    dots::Subscription m_subscription;
};
