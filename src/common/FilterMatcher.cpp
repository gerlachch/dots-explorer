#include <common/FilterMatcher.h>
#include <fmt/format.h>

FilterMatcher::FilterMatcher(const Filter& filter) :
    m_regex{ std::make_unique<re2::RE2>(fmt::format("{}{}", filter.matchCase == true ? "" : "(?i)", *filter.expression), RE2::Options{ RE2::Quiet })}
{
    if (!m_regex->ok())
    {
        throw std::runtime_error{ "invalid regex input: " + m_regex->error() };
    };
}

bool FilterMatcher::match(std::string_view str) const
{
    return RE2::PartialMatch(str, *m_regex);
}
