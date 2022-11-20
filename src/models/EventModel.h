#pragma once
#include <models/MetadataModel.h>
#include <models/StructModel.h>
#include <FilterSettings.dots.h>

struct EventModel
{
    EventModel(size_t index, MetadataModel metadataModel, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event);

    size_t index() const;
    const ImGuiExt::ColoredText& indexText() const;

    size_t instanceId() const;

    const MetadataModel& metadataModel() const;
    const StructModel& publishedInstanceModel() const;
    const StructModel& updatedInstanceModel() const;

private:

    struct data
    {
        size_t index;
        ImGuiExt::ColoredText indexText;
        MetadataModel metadataModel;
        StructModel publishedInstanceModel;
        StructModel updatedInstanceModel;
        size_t instanceId;
    };

    std::shared_ptr<const data> m_data;
};
