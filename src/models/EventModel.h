#pragma once
#include <models/MetadataModel.h>
#include <models/StructModel.h>
#include <FilterSettings.dots.h>

struct EventModel
{
    EventModel(size_t index, const PublisherModel& publisherModel, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event);
    EventModel(const EventModel& other) = delete;
    EventModel(EventModel&& other) = default;
    ~EventModel() = default;

    EventModel& operator = (const EventModel& rhs) = delete;
    EventModel& operator = (EventModel&& rhs) = default;

    size_t index() const;
    const ImGuiExt::ColoredText& indexText() const;

    size_t instanceId() const;

    const MetadataModel& metadataModel() const;
    const StructModel& publishedInstanceModel() const;
    const StructModel& updatedInstanceModel() const;

private:

    size_t m_index;
    ImGuiExt::ColoredText m_indexText;
    MetadataModel m_metadataModel;
    StructModel m_publishedInstanceModel;
    StructModel m_updatedInstanceModel;
    size_t m_instanceId;
};

using event_model_ptr_t = std::shared_ptr<const EventModel>;
