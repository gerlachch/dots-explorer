#pragma once
#include <dots/dots.h>
#include <imgui.h>
#include <widgets/PropertyDescription.h>

struct ImGuiTableColumnSortSpecs;

struct PropertyView
{
    PropertyView(const dots::type::ProxyProperty<>& property);
    PropertyView(const PropertyView& other) = delete;
    PropertyView(PropertyView&& other) = default;
    ~PropertyView() = default;

    PropertyView& operator = (const PropertyView& rhs) = delete;
    PropertyView& operator = (PropertyView&& rhs) = default;

    const dots::type::ProxyProperty<>& property() const;
    bool less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyView& other) const;
    bool isSelected() const;

    void update();
    void render(const PropertyDescription& propertyDescription);

private:

    std::string m_value;
    bool m_isSelected;
    const dots::type::ProxyProperty<> m_property;
};
