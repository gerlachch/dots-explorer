#pragma once
#include <vector>
#include <optional>
#include <common/FilterMatcher.h>
#include <models/TransceiverModel.h>
#include <widgets/dialogs/FilterSettingsEdit.h>
#include <widgets/views/StructList.h>
#include <widgets/input/FilterExpressionEdit.h>
#include <FilterSettings.dots.h>

struct CacheView
{
    CacheView(TransceiverModel& transceiverModel);
    CacheView(const CacheView& other) = delete;
    CacheView(CacheView&& other) = delete;
    ~CacheView() = default;

    CacheView& operator = (const CacheView& rhs) = delete;
    CacheView& operator = (CacheView&& rhs) = delete;

    void render();

private:

    static constexpr uint32_t NoFilterSelected = std::numeric_limits<uint32_t>::max();

    void initFilterSettings();
    void applyFilters();

    void renderFilterArea();
    void renderCacheList();

    std::optional<FilterExpressionEdit> m_filterExpressionEdit;
    std::optional<FilterSettingsEdit> m_filterSettingsEdit;
    std::vector<std::shared_ptr<StructList>> m_cacheList;
    std::vector<std::shared_ptr<StructList>> m_cacheListFiltered;
    std::optional<FilterMatcher> m_filterMatcher;
    bool m_typesChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
};
