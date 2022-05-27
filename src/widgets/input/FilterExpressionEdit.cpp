#include <widgets/input/FilterExpressionEdit.h>
#include <imgui.h>
#include <fmt/format.h>
#include <common/FilterMatcher.h>
#include <common/Colors.h>

FilterExpressionEdit::FilterExpressionEdit(Filter& filter, std::string hint/* = "<none>"*/) :
    m_label{ fmt::format("##FilterExpressionEdit-{}", ++M_id) },
    m_isValid(true),
    m_buffer(256, '\0'),
    m_filter(&filter),
    m_hint{ std::move(hint) }
{
    m_buffer.assign(std::max(filter.expression->size(), m_buffer.size()), '\0');
    std::copy(filter.expression->begin(), filter.expression->end(), m_buffer.begin());
    fetch(ImGui::GetStyleColorVec4(ImGuiCol_Text));
}

bool FilterExpressionEdit::isValid() const
{
    return m_isValid;
}

bool FilterExpressionEdit::render()
{
    bool filterChanged = false;
    ImVec4 regularTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    ImGui::PushStyleColor(ImGuiCol_Text, m_inputColor);
    if (ImGui::InputTextWithHint(m_label.data(), m_hint.data(), m_buffer.data(), m_buffer.size()))
    {
        fetch(regularTextColor);
        filterChanged = m_isValid;
    }
    ImGui::PopStyleColor();

    return filterChanged;
}

void FilterExpressionEdit::fetch(const ImVec4& regularTextColor)
{
    try
    {
        FilterMatcher filterMatcher{ *m_filter->expression, false };
        m_filter->expression = m_buffer.data();
        m_inputColor = regularTextColor;
        m_isValid = true;
    }
    catch (...)
    {
        m_inputColor = ColorThemeActive.Error;
        m_isValid = false;
    }
}
