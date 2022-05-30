#pragma once
#include <common/ImGuiExt.h>
#include <Filter.dots.h>

struct FilterExpressionEdit
{
    FilterExpressionEdit(Filter& filter, std::string hint = "<none>");

    bool isValid() const;
    bool render();

private:

    inline static uint64_t M_id = 0;

    void fetch(const ImVec4& regularTextColor);

    ImVec4 m_inputColor;
    std::string m_label;
    bool m_isValid;
    std::string m_buffer;
    Filter* m_filter;
    std::string m_hint;
};
