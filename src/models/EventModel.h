#pragma once
#include <common/ImGuiExt.h>
#include <dots/dots.h>
#include <models/StructModel.h>
#include <models/MetadataModel.h>

struct EventModel
{
    EventModel(size_t index, const StructDescriptorModel& descriptorModel, const dots::Event<>& event);

    const StructModel& structModel() const;
    const MetadataModel& metadataModel() const;

    const ImGuiExt::ColoredText& indexText() const;
    const ImGuiExt::ColoredText& publishedInstanceTextRaw() const;
    const std::vector<ImGuiExt::ColoredText>& publishedInstanceTextSimple() const;
    const std::vector<ImGuiExt::ColoredText>& publishedInstanceTextFancy() const;

    void fetch();

private:

    MetadataModel m_metadataModel;
    mutable ImGuiExt::ColoredText m_indexText;
    mutable ImGuiExt::ColoredText m_publishedInstanceText;
    mutable std::vector<ImGuiExt::ColoredText> m_publishedInstanceTextFancy;
    size_t m_index;
    dots::type::AnyStruct m_publishedInstance;
    StructModel m_structModel;
};