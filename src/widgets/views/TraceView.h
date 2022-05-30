#pragma once
#include <vector>
#include <optional>
#include <unordered_map>
#include <dots/dots.h>
#include <common/FilterMatcher.h>
#include <widgets/views/TraceItem.h>
#include <widgets/dialogs/FilterSettingsEdit.h>
#include <widgets/dialogs/PublishDialog.h>
#include <widgets/input/FilterExpressionEdit.h>
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
    void applyFilters();

    void renderFilterArea();
    void renderEventList();

    std::optional<FilterExpressionEdit> m_filterExpressionEdit;
    std::optional<FilterSettingsEdit> m_filterSettingsEdit;
    std::optional<PublishDialog> m_publishDialog;
    std::unordered_map<const dots::type::StructDescriptor<>*, StructDescriptorModel> m_descriptorModels;
    std::vector<std::shared_ptr<TraceItem>> m_items;
    std::vector<std::shared_ptr<TraceItem>> m_itemsFiltered;
    std::optional<FilterMatcher> m_filterMatcher;
    PublisherModel m_publisherModel;
    size_t m_traceIndex;
    bool m_filtersChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    float m_pageScrollTotalTime;
    float m_pageScrollDeltaTime;
    float m_sidewaysScrollTotalTime;
    float m_sidewaysScrollDeltaTime;
    std::vector<dots::Subscription> m_subscriptions;
};
