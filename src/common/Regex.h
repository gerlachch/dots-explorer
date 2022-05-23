#pragma once
#include <any>
#include <string_view>

struct Regex
{
    Regex(std::string_view regex, bool matchCase = true);

    bool search(std::string_view str);

private:

    std::any m_regex;
};
