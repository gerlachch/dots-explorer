#include <models/PublisherModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

PublisherModel::PublisherModel(std::shared_ptr<std::map<uint32_t, ImGuiExt::ColoredText>> publisherNameTexts) :
    m_publisherNameTexts(std::move(publisherNameTexts))
{
    /* do nothing */
}

const ImGuiExt::ColoredText& PublisherModel::publisherNameText(uint32_t id) const
{
    if (auto it = m_publisherNameTexts->find(id); it == m_publisherNameTexts->end())
        return m_publisherNameTexts->try_emplace(id, fmt::format("\"<unknown> [{}]\"", id), ColorThemeActive.StringType).first->second;
    else
        return it->second;
}
