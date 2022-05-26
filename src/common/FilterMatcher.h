#pragma once
#include <string_view>
#include <memory>
#include <re2/re2.h>

struct FilterMatcher
{
    FilterMatcher(std::string_view expression, bool matchCase = true);
    FilterMatcher(const FilterMatcher& other) = delete;
    FilterMatcher(FilterMatcher&& other) = default;
    ~FilterMatcher() = default;

    FilterMatcher& operator = (const FilterMatcher& rhs) = delete;
    FilterMatcher& operator = (FilterMatcher&& rhs) = default;

    bool match(std::string_view str) const;

private:

    std::unique_ptr<RE2> m_regex;
};
