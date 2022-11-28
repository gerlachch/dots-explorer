#pragma once
#include <Host.dots.h>

struct HostEndpointEdit
{
    HostEndpointEdit(Host& host, bool initWithDescription = true, std::string hint = "<host-endpoint>");

    bool render();

private:

    inline static uint64_t M_id = 0;

    std::string m_buffer;
    std::string m_label;
    Host* m_host;
    std::string m_hint;
};
