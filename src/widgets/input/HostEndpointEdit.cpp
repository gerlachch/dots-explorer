#include <widgets/input/HostEndpointEdit.h>
#include <imgui.h>
#include <fmt/format.h>

HostEndpointEdit::HostEndpointEdit(Host& host, bool initWithDescription/* = true*/, std::string hint/* = "<host-endpoint>"*/) :
    m_label{ fmt::format("##HostEndpointEdit-{}", ++M_id) },
    m_host(&host),
    m_hint{ std::move(hint) }
{
    std::string initialText = initWithDescription ? *m_host->description : *m_host->endpoint;
    initialText += '\0';
    m_buffer.assign(std::max(initialText.size(), size_t{ 1024 }), '\0');
    std::copy(initialText.begin(), initialText.end(), m_buffer.begin());
}

bool HostEndpointEdit::render()
{
    bool hostChanged = false;

    if (ImGui::InputTextWithHint(m_label.data(), m_hint.data(), m_buffer.data(), m_buffer.size(), ImGuiInputTextFlags_AutoSelectAll))
    {
        m_host->endpoint = m_buffer.data();
        hostChanged = true;
    }

    return hostChanged;
}
