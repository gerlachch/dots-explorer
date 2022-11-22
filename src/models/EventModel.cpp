#include <models/EventModel.h>
#include <fmt/format.h>
#include <common/Colors.h>

EventModel::EventModel(size_t index, size_t updateIndex, MetadataModel metadataModel, const StructDescriptorModel& descriptorModel, const dots::Event<>& event) :
    m_data(std::make_shared<data>(data{
        .index = index,
        .indexText = { fmt::format("#{}", index), ColorThemeActive.Disabled },
        .updateIndex = updateIndex,
        .metadataModel = std::move(metadataModel),
        .publishedInstanceModel = { descriptorModel, event.transmitted() },
        .updatedInstanceModel = { descriptorModel, event.updated() }
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

size_t EventModel::updateIndex() const
{
    return m_data->updateIndex;
}

const StructDescriptorModel& EventModel::descriptorModel() const
{
    return m_data->publishedInstanceModel.descriptorModel();
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
