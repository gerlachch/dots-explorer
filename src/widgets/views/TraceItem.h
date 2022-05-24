#pragma once
#include <dots/dots.h>
#include <common/Regex.h>
#include <models/MetadataModel.h>
#include <models/StructRefModel.h>
#include <FilterSettings.dots.h>

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
    const MetadataModel& metadataModel() const;
    const StructRefModel& publishedInstanceModel() const;
    const StructRefModel& updatedInstanceModel() const;

    bool isSelected() const;
    bool isHovered() const;
    bool isFiltered(const std::optional<Regex>& filter, const FilterSettings& filterSettings) const;

    void render(bool hoverCondition);

private:

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    bool m_isSelected;
    bool m_isHovered;
    size_t m_index;
    ImGuiExt::ColoredText m_indexText;
    dots::type::AnyStruct m_publishedInstance;
    dots::type::AnyStruct m_updatedInstance;
    MetadataModel m_metadataModel;
    StructRefModel m_publishedInstanceModel;
    StructRefModel m_updatedInstanceModel;
};
