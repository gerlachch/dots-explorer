#pragma once
#include <vector>
#include <optional>
#include <dots/dots.h>
#include <widgets/FilterSettingsEdit.h>
#include <widgets/StructList.h>
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

    void renderFilterArea();
    void renderCacheList();

    std::optional<FilterSettingsEdit> m_filterSettingsEdit;
    std::vector<std::shared_ptr<StructList>> m_cacheList;
    std::vector<std::shared_ptr<StructList>> m_cacheListFiltered;
    std::string m_typeFilterBuffer;
    bool m_typesChanged;
    bool m_filterSettingsInitialized;
    FilterSettings& m_filterSettings;
    std::vector<dots::Subscription> m_subscriptions;
};
