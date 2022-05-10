#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <regex>
#include <dots/dots.h>
#include <widgets/TraceItem.h>
#include <widgets/FilterSettingsEdit.h>
#include <widgets/StructEdit.h>
#include <FilterSettings.dots.h>

struct TraceView
{
    TraceView();
    TraceView(const TraceView& other) = delete;
    TraceView(TraceView&& other) = delete;
    ~TraceView() = default;

    TraceView& operator = (const TraceView& rhs) = delete;
    TraceView& operator = (TraceView&& rhs) = delete;

    void update(const dots::type::StructDescriptor<>& descriptor);
    void render();

private:

    void initFilterSettings();
    bool applyFilter(const TraceItem& item);
    void applyFilters();

    void renderFilterArea();
    void renderEventList();

    std::optional<FilterSettingsEdit> m_filterSettingsEdit;
    std::optional<StructEdit> m_structEdit;
    std::unordered_map<const dots::type::StructDescriptor<>*, StructDescriptorModel> m_descriptorModels;
    std::vector<std::shared_ptr<TraceItem>> m_items;
    std::vector<std::shared_ptr<TraceItem>> m_itemsFiltered;
    std::optional<std::regex> m_regex;
    size_t m_traceIndex;
    std::string m_eventFilterBuffer;
    bool m_filtersChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    std::vector<dots::Subscription> m_subscriptions;
};
