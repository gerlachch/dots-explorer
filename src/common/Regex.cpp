#include <common/Regex.h>
#include <regex>

Regex::Regex(std::string_view regex, bool matchCase/* = true*/) :
    m_regex{ std::make_any<std::regex>(std::string{ regex }, matchCase ? std::regex_constants::ECMAScript : std::regex_constants::ECMAScript | std::regex_constants::icase) }
{
    /* do nothing */
}

bool Regex::search(std::string_view str)
{
    auto& regex = std::any_cast<const std::regex&>(m_regex);
    return std::regex_search(std::string{ str }, regex);
}
