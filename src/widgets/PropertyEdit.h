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

    using header_part_t = std::pair<std::string, ImVec4>;

    std::string m_buffer;
    std::optional<ImVec4> m_color;
    std::optional<bool> m_inputParseable;
    std::vector<header_part_t> m_descriptionParts;
    dots::type::ProxyProperty<> m_property;
    std::string m_inputLabel;
    std::string m_invalidateLabel;
};
