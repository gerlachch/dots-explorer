#include <models/EventModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

EventModel::EventModel(size_t index, const PublisherModel& publisherModel, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event) :
    m_index(index),
    m_indexText{ fmt::format("#{}", m_index), ColorThemeActive.Disabled },
    m_metadataModel{ publisherModel },
    m_publishedInstanceModel{ structDescriptorModel, event.transmitted() },
    m_updatedInstanceModel{ structDescriptorModel, event.updated() },
    m_instanceId(reinterpret_cast<size_t>(&event.updated()))
{
    m_metadataModel.fetch(event);
}

size_t EventModel::index() const
{
    return m_index;
}

const ImGuiExt::ColoredText& EventModel::indexText() const
{
    return m_indexText;
}

size_t EventModel::instanceId() const
{
    return m_instanceId;
}

const MetadataModel& EventModel::metadataModel() const
{
    return m_metadataModel;
}
const StructModel& EventModel::publishedInstanceModel() const
{
    return m_publishedInstanceModel;
}

const StructModel& EventModel::updatedInstanceModel() const
{
    return m_updatedInstanceModel;
}
