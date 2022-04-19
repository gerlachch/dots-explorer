#pragma once
#include <string>
#include <optional>
#include <dots/dots.h>
#include <imgui.h>

struct PropertyEdit
{
    PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath);
    PropertyEdit(const PropertyEdit& other) = delete;
    PropertyEdit(PropertyEdit&& other) = default;
    ~PropertyEdit() = default;

    PropertyEdit& operator = (const PropertyEdit& rhs) = delete;
    PropertyEdit& operator = (PropertyEdit&& rhs) = default;

    std::optional<bool> inputParseable() const;

    void render();

private:

    std::string m_buffer;
    std::optional<ImVec4> m_color;
    std::optional<bool> m_inputParseable;
    std::string m_header;
    dots::type::ProxyProperty<> m_property;
    std::string m_inputLabel;
    std::string m_invalidateLabel;
};
