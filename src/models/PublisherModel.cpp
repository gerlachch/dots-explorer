#include <models/PublisherModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

PublisherModel::PublisherModel() :
    m_subscription(dots::subscribe<DotsClient>({ &PublisherModel::handleDotsClient, this }))
{
    m_publisherNameTexts.try_emplace(dots::Connection::HostId, fmt::format("\"{}\"", dots::global_transceiver()->connection().peerName()), ColorThemeActive.StringType).first->second;
}

const ImGuiExt::ColoredText& PublisherModel::publisherNameText(uint32_t id) const
{
    if (auto it = m_publisherNameTexts.find(id); it == m_publisherNameTexts.end())
    {
        return m_publisherNameTexts.try_emplace(id, fmt::format("\"<unknown> [{}]\"", id), ColorThemeActive.StringType).first->second;
    }
    else
    {
        return it->second;
    }
}

ImGuiExt::ColoredText& PublisherModel::publisherNameText(uint32_t id)
{
    return const_cast<ImGuiExt::ColoredText&>(std::as_const(*this).publisherNameText(id));
}

void PublisherModel::handleDotsClient(const dots::Event<DotsClient>& event)
{
    if (const auto& client = event(); client.name.isValid())
    {
        publisherNameText(*client.id).first = fmt::format("\"{}\"", *client.name);
    }
}
