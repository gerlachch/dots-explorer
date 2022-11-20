#pragma once
#include <map>
#include <common/ImGuiExt.h>
#include <dots/dots.h>

struct PublisherModel
{
    PublisherModel(std::shared_ptr<std::map<uint32_t, ImGuiExt::ColoredText>> publisherNameTexts);
    const ImGuiExt::ColoredText& publisherNameText(uint32_t id) const;

private:

    std::shared_ptr<std::map<uint32_t, ImGuiExt::ColoredText>> m_publisherNameTexts;
};
