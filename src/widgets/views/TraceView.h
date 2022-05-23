#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <regex>
#include <dots/dots.h>
#include <widgets/views/TraceItem.h>
#include <widgets/dialogs/FilterSettingsEdit.h>
#include <widgets/dialogs/PublishDialog.h>
#include <widgets/input/RegexEdit.h>
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
    std::optional<PublishDialog> m_publishDialog;
    std::unordered_map<const dots::type::StructDescriptor<>*, StructDescriptorModel> m_descriptorModels;
    std::vector<std::shared_ptr<TraceItem>> m_items;
    std::vector<std::shared_ptr<TraceItem>> m_itemsFiltered;
    std::optional<std::regex> m_regex;
    PublisherModel m_publisherModel;
    RegexEdit m_filterEdit;
    size_t m_traceIndex;
    bool m_filtersChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    float m_pageScrollTime;
    std::vector<dots::Subscription> m_subscriptions;
};
