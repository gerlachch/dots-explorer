#pragma once
#include <optional>
#include <dots/dots.h>
#include <imgui.h>

struct ImGuiTableColumnSortSpecs;

struct PropertyView
{
    PropertyView(dots::type::ProxyProperty<> property);
    PropertyView(const PropertyView& other) = delete;
    PropertyView(PropertyView&& other) = default;
    ~PropertyView() = default;

    PropertyView& operator = (const PropertyView& rhs) = delete;
    PropertyView& operator = (PropertyView&& rhs) = default;

    dots::type::ProxyProperty<> property() const;
    bool less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyView& other) const;

    void update();
    void render();

private:

    std::string m_value;
    std::optional<ImVec4> m_color;
    bool m_valueQuoted;
    dots::type::ProxyProperty<> m_property;
};
