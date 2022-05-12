#include <models/MetadataModel.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <DotsClient.dots.h>

MetadataModel::MetadataModel() :
    m_lastOperation(DotsMt::create),
    m_lastPublishedBy(0),
    m_metadataText(MetaDataSize, ImGuiExt::ColoredText{})
{
    /* do nothing */
}

DotsMt MetadataModel::lastOperation() const
{
    return m_lastOperation;
}

dots::timepoint_t MetadataModel::lastPublished() const
{
    return m_lastPublished;
}

dots::uint32_t MetadataModel::lastPublishedBy() const
{
    return m_lastPublishedBy;
}

const std::vector<ImGuiExt::ColoredText>& MetadataModel::metadataText() const
{
    return m_metadataText;
}

const ImGuiExt::ColoredText& MetadataModel::lastOperationText() const
{
    return LastOperationTexts[static_cast<size_t>(m_lastOperation)];
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedText() const
{
    const ImGuiExt::ColoredText& coloredText = m_metadataText[LastPublished];

    if (coloredText.first.empty())
    {
        auto& [text, color] = m_metadataText[LastPublished];
        text = m_lastPublished.toString("%F %T");
        color = ColorThemeActive.IntegralType;
    }

    return coloredText;
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedByText() const
{
    const ImGuiExt::ColoredText& coloredText = m_metadataText[LastPublishedBy];

    if (coloredText.first.empty())
    {
        auto& [text, color] = m_metadataText[LastPublishedBy];

        if (const auto* client = dots::container<DotsClient>().find(DotsClient::id_i{ m_lastPublishedBy }); client == nullptr || !client->name.isValid())
        {
            text = fmt::format("\"<unknown> [{}]\"", m_lastPublishedBy);
        }
        else
        {
            text = fmt::format("\"{}\"", *client->name);
        }

        color = ColorThemeActive.StringType;
        
    }

    return coloredText;
}

void MetadataModel::fetch(const dots::Event<>& event)
{
    m_lastOperation = event.mt();
    m_lastPublished = event.cloneInfo().modified.valueOrDefault(event.cloneInfo().created);
    m_lastPublishedBy = event.cloneInfo().lastUpdateFrom.valueOrDefault(event.cloneInfo().createdFrom);

    for (auto& [text, color] : m_metadataText)
    {
        text.clear();
    }
}
