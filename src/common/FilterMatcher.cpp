#include <common/FilterMatcher.h>
#include <dots/tools/string_tools.h>
#include <fmt/format.h>

FilterMatcher::FilterMatcher(const Filter& filter) :
    m_quickFilterDefault(false)
{
    std::string expression{ *filter.expression };

    if (!filter.matchCase)
    {
        std::transform(expression.begin(), expression.end(), expression.begin(), std::tolower);
    }

    if (filter.regex)
    {
        m_regex = std::make_unique<RE2>(expression, RE2::Options{ RE2::Quiet });

        if (!m_regex->ok())
        {
            throw std::runtime_error{ "invalid regex input: " + m_regex->error() };
        }
    }
    else
    {
        auto split = [](std::string_view str, std::string_view delimiter)
        {
            return dots::tools::split_left_at_first_of(str, delimiter, false);
        };

        quick_filter_t whitelist;

        for (auto [sub, tail] = split(expression, "|"); !sub.empty(); std::tie(sub, tail) = split(tail, "|"))
        {
            if (bool inverted = sub.front() == '!'; inverted)
            {
                sub.remove_prefix(1);
                m_quickFilter.emplace_back(sub, inverted);
            }
            else
            {
                whitelist.emplace_back(sub, inverted);
            }
        }

        m_quickFilter.insert(m_quickFilter.end(), whitelist.begin(), whitelist.end());

        if (!m_quickFilter.empty())
        {
            m_quickFilterDefault = m_quickFilter.back().second;
        }
    }
}

bool FilterMatcher::match(std::string_view str) const
{
    if (m_regex)
    {
        return RE2::PartialMatch(str, *m_regex);

    }
    else
    {
        for (const auto& [sub, inverted] : m_quickFilter)
        {
            if (str.find(sub) != std::string::npos)
            {
                return !inverted;
            }
        }

        return m_quickFilterDefault;
    }
}
