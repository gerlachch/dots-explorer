#pragma once
#include <common/ImGuiExt.h>
#include <dots/dots.h>
#include <models/StructModel.h>
#include <models/MetadataModel.h>

struct EventModel
{
    EventModel(size_t index, const StructDescriptorModel& descriptorModel, const dots::Event<>& event);

    size_t index() const;
    const StructModel& structModel() const;
    const MetadataModel& metadataModel() const;

    const ImGuiExt::ColoredText& indexText() const;

private:

    MetadataModel m_metadataModel;
    mutable ImGuiExt::ColoredText m_indexText;
    size_t m_index;
    dots::type::AnyStruct m_publishedInstance;
    StructModel m_structModel;
};
