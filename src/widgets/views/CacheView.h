#pragma once
#include <vector>
#include <optional>
#include <dots/dots.h>
#include <common/FilterMatcher.h>
#include <widgets/dialogs/FilterSettingsEdit.h>
#include <widgets/views/StructList.h>
#include <widgets/input/FilterExpressionEdit.h>
#include <FilterSettings.dots.h>

struct CacheView
{
    CacheView();
    CacheView(const CacheView& other) = delete;
    CacheView(CacheView&& other) = delete;
    ~CacheView() = default;

    CacheView& operator = (const CacheView& rhs) = delete;
    CacheView& operator = (CacheView&& rhs) = delete;

    void update(const dots::type::StructDescriptor<>& descriptor);
    void render();

private:

    void initFilterSettings();
    bool applyFilter(const StructList& structList);
    void applyFilters();

    void renderFilterArea();
    void renderCacheList();

    std::optional<FilterExpressionEdit> m_filterExpressionEdit;
    std::optional<FilterSettingsEdit> m_filterSettingsEdit;
    std::vector<std::shared_ptr<StructList>> m_cacheList;
    std::vector<std::shared_ptr<StructList>> m_cacheListFiltered;
    std::optional<FilterMatcher> m_filterMatcher;
    PublisherModel m_publisherModel;
    bool m_typesChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    std::vector<dots::Subscription> m_subscriptions;
};
