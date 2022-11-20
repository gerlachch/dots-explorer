#include <models/MetadataModel.h>
#include <fmt/format.h>
#include <common/Colors.h>
#include <DotsClient.dots.h>

MetadataModel::MetadataModel(const PublisherModel& publisherModel, const dots::Event<>& event) :
    m_data(std::make_shared<data>(data{
        .lastOperation = event.mt(),
        .lastPublished = event.cloneInfo().modified.valueOrDefault(*event.cloneInfo().created),
        .lastPublishedBy = event.cloneInfo().lastUpdateFrom.valueOrDefault(*event.cloneInfo().createdFrom),
        .lastPublishedProperties = event.header().attributes.valueOrDefault(dots::property_set_t::All),
        .lastPublishedText = { event.cloneInfo().modified.valueOrDefault(*event.cloneInfo().created).toString("%F %T"), ColorThemeActive.IntegralType },
        .publisherModel = publisherModel
    }))
{
    /* do nothing */
}

DotsMt MetadataModel::lastOperation() const
{
    return m_data->lastOperation;
}

dots::timepoint_t MetadataModel::lastPublished() const
{
    return m_data->lastPublished;
}

dots::uint32_t MetadataModel::lastPublishedBy() const
{
    return m_data->lastPublishedBy;
}

dots::property_set_t MetadataModel::lastPublishedProperties() const
{
    return m_data->lastPublishedProperties;
}

const ImGuiExt::ColoredText& MetadataModel::lastOperationText() const
{
    return LastOperationTexts[static_cast<size_t>(m_data->lastOperation)];
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedText() const
{
    return m_data->lastPublishedText;
}

const ImGuiExt::ColoredText& MetadataModel::lastPublishedByText() const
{
    return m_data->publisherModel.publisherNameText(m_data->lastPublishedBy);
}
