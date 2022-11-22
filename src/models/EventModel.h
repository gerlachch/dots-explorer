#pragma once
#include <models/MetadataModel.h>
#include <models/StructModel.h>
#include <FilterSettings.dots.h>

struct EventModel
{
    EventModel(size_t index, size_t updateIndex, MetadataModel metadataModel, const StructDescriptorModel& descriptorModel, const dots::Event<>& event);

    size_t index() const;
    const ImGuiExt::ColoredText& indexText() const;

    size_t updateIndex() const;

    const StructDescriptorModel& descriptorModel() const;
    const MetadataModel& metadataModel() const;
    const StructModel& publishedInstanceModel() const;
    const StructModel& updatedInstanceModel() const;

private:

    struct data
    {
        size_t index;
        ImGuiExt::ColoredText indexText;
        size_t updateIndex;
        MetadataModel metadataModel;
        StructModel publishedInstanceModel;
        StructModel updatedInstanceModel;
    };

    std::shared_ptr<const data> m_data;
};
