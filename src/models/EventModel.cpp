#include <models/EventModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

EventModel::EventModel(size_t index, const StructDescriptorModel& descriptorModel, const PublisherModel& publisherModel, const dots::Event<>& event) :
    m_index(index),
    m_publishedInstance(event.transmitted()),
    m_metadataModel{ publisherModel },
    m_structModel{ descriptorModel, *m_publishedInstance }
{
    m_metadataModel.fetch(event);
}

size_t EventModel::index() const
{
    return m_index;
}

const StructModel& EventModel::structModel() const
{
    return m_structModel;
}

const MetadataModel& EventModel::metadataModel() const
{
    return m_metadataModel;
}

const ImGuiExt::ColoredText& EventModel::indexText() const
{
    if (m_indexText.first.empty())
    {
        m_indexText = { fmt::format("#{}", m_index), ColorThemeActive.Disabled};
    }

    return m_indexText;
}
