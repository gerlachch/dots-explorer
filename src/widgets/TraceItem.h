#pragma once
#include <dots/dots.h>
#include <models/EventModel.h>

struct ImGuiTableSortSpecs;

struct TraceItem
{
    TraceItem(size_t index, const StructDescriptorModel& structDescriptorModel, const dots::Event<>& event);
    TraceItem(const TraceItem& other) = delete;
    TraceItem(TraceItem&& other) = default;
    ~TraceItem() = default;

    TraceItem& operator = (const TraceItem& rhs) = delete;
    TraceItem& operator = (TraceItem&& rhs) = default;

    const char* widgetId() const;

    const EventModel& eventModel() const ;
    EventModel& eventModel();

    bool isSelected() const;
    bool isHovered() const;

    void render(bool hoverCondition);

private:

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    bool m_isSelected;
    bool m_isHovered;
    EventModel m_eventModel;
};
