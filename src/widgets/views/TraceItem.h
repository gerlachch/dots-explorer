#pragma once
#include <dots/dots.h>
#include <models/MetadataModel.h>
#include <models/StructRefModel.h>

struct ImGuiTableSortSpecs;

struct TraceItem
{
    TraceItem(size_t index, const StructDescriptorModel& structDescriptorModel, const PublisherModel& publisherModel, const dots::Event<>& event);
    TraceItem(const TraceItem& other) = delete;
    TraceItem(TraceItem&& other) = default;
    ~TraceItem() = default;

    TraceItem& operator = (const TraceItem& rhs) = delete;
    TraceItem& operator = (TraceItem&& rhs) = default;

    const char* widgetId() const;

    size_t index() const;
    const StructRefModel& structRefModel() const;
    const MetadataModel& metadataModel() const;

    bool isSelected() const;
    bool isHovered() const;

    void render(bool hoverCondition);

private:

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    bool m_isSelected;
    bool m_isHovered;
    size_t m_index;
    ImGuiExt::ColoredText m_indexText;
    dots::type::AnyStruct m_publishedInstance;
    MetadataModel m_metadataModel;
    StructRefModel m_structRefModel;
};
