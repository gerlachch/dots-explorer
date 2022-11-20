#include <models/EventModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

EventModel::EventModel(size_t index, MetadataModel metadataModel, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event) :
    m_data(std::make_shared<data>(data{
        .index = index,
        .indexText = { fmt::format("#{}", index), ColorThemeActive.Disabled },
        .metadataModel = std::move(metadataModel),
        .publishedInstanceModel = { structDescriptorModel, event.transmitted() },
        .updatedInstanceModel = { structDescriptorModel, event.updated() },
        .instanceId = reinterpret_cast<size_t>(&event.updated())
    }))
{
    /* do nothing */
}

size_t EventModel::index() const
{
    return m_data->index;
}

const ImGuiExt::ColoredText& EventModel::indexText() const
{
    return m_data->indexText;
}

size_t EventModel::instanceId() const
{
    return m_data->instanceId;
}

const MetadataModel& EventModel::metadataModel() const
{
    return m_data->metadataModel;
}
const StructModel& EventModel::publishedInstanceModel() const
{
    return m_data->publishedInstanceModel;
}

const StructModel& EventModel::updatedInstanceModel() const
{
    return m_data->updatedInstanceModel;
}
