#pragma once
#include <dots/dots.h>
#include <common/FilterMatcher.h>
#include <models/EventModel.h>

struct ImGuiTableSortSpecs;

struct TraceItem
{
    TraceItem(EventModel model);

    const char* widgetId() const;
    const EventModel& model() const;

    bool isSelected() const;
    bool isHovered() const;

    void setFilterTargets(const FilterTargets& targets);
    bool isFiltered(const std::optional<FilterMatcher>& filter, const FilterSettings& filterSettings) const;

    void render(bool hoverCondition);

private:

    inline static uint64_t M_nextWidgetId = 0;

    mutable std::string m_widgetId;
    std::string m_filterText;
    std::string m_filterTextLower;
    bool m_isSelected;
    bool m_isHovered;
    EventModel m_model;
};
