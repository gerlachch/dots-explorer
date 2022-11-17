#include <models/MetadataModel.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <DotsClient.dots.h>

MetadataModel::MetadataModel(const PublisherModel& publisherModel) :
    m_lastOperation(DotsMt::create),
    m_lastPublishedBy(0),
    m_lastPublishedProperties{ dots::property_set_t::None },
    m_publisherModel{ publisherModel }
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

dots::property_set_t MetadataModel::lastPublishedProperties() const
{
    return m_lastPublishedProperties;
}

const ImGuiExt::ColoredText& MetadataModel::lastOperationText() const
{
    return LastOperationTexts[static_cast<size_t>(m_lastOperation)];
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedText() const
{
    if (auto& [text, color] = m_lastPublishedText; text.empty())
    {
        text = m_lastPublished.toString("%F %T");
        color = ColorThemeActive.IntegralType;
    }

    return m_lastPublishedText;
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedByText() const
{
    return m_publisherModel.get().publisherNameText(m_lastPublishedBy);
}

void MetadataModel::fetch(const dots::Event<>& event)
{
    m_lastOperation = event.mt();
    m_lastPublished = event.cloneInfo().modified.valueOrDefault(*event.cloneInfo().created);
    m_lastPublishedBy = event.cloneInfo().lastUpdateFrom.valueOrDefault(*event.cloneInfo().createdFrom);
    m_lastPublishedProperties = event.header().attributes.valueOrDefault(dots::property_set_t::All);

    m_lastPublishedText.first.clear();
}
