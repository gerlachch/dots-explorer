#pragma once
#include <dots/dots.h>
#include <FilterSettings.dots.h>

struct FilterSettingsEdit
{
    FilterSettingsEdit(FilterSettings& settings, Filter* editFilter = nullptr);
    FilterSettingsEdit(const FilterSettingsEdit& other) = delete;
    FilterSettingsEdit(FilterSettingsEdit&& other) = delete;
    ~FilterSettingsEdit() = default;

    FilterSettingsEdit& operator = (const FilterSettingsEdit& rhs) = delete;
    FilterSettingsEdit& operator = (FilterSettingsEdit&& rhs) = delete;

    bool render();

private:

    inline static uint64_t M_id;

    std::string m_popupId;
    std::string m_regexBuffer;
    std::string m_descriptionBuffer;
    const char* m_headerText;
    FilterSettings& m_settings;
    Filter* m_editFilter;
};
