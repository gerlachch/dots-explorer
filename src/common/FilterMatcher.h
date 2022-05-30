#pragma once
#include <string_view>
#include <vector>
#include <memory>
#include <re2/re2.h>
#include <Filter.dots.h>

struct FilterMatcher
{
    FilterMatcher(const Filter& filter);
    FilterMatcher(const FilterMatcher& other) = delete;
    FilterMatcher(FilterMatcher&& other) = default;
    ~FilterMatcher() = default;

    FilterMatcher& operator = (const FilterMatcher& rhs) = delete;
    FilterMatcher& operator = (FilterMatcher&& rhs) = default;

    bool match(std::string_view str) const;

private:

    using quick_filter_part_t = std::pair<std::string, bool>;
    using quick_filter_t = std::vector<quick_filter_part_t>;

    bool m_quickFilterDefault;
    std::unique_ptr<RE2> m_regex;
    quick_filter_t m_quickFilter;
};
