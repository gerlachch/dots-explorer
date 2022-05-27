#pragma once
#include <optional>
#include <dots/dots.h>
#include <widgets/input/FilterExpressionEdit.h>
#include <FilterSettings.dots.h>

struct FilterSettingsEdit
{
    FilterSettingsEdit(FilterSettings& settings, Filter* existingFilter = nullptr);
    FilterSettingsEdit(const FilterSettingsEdit& other) = delete;
    FilterSettingsEdit(FilterSettingsEdit&& other) = delete;
    ~FilterSettingsEdit() = default;

    FilterSettingsEdit& operator = (const FilterSettingsEdit& rhs) = delete;
    FilterSettingsEdit& operator = (FilterSettingsEdit&& rhs) = delete;

    bool render();

private:

    inline static uint64_t M_id;

    Filter m_filter;
    std::optional<FilterExpressionEdit> m_filterExpressionEdit;
    std::string m_popupId;
    std::string m_descriptionBuffer;
    const char* m_headerText;
    FilterSettings& m_settings;
    Filter* m_existingFilter;
};
