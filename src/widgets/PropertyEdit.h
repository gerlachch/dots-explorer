#pragma once
#include <string>
#include <optional>
#include <dots/dots.h>

struct PropertyEdit
{
    PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath);
    PropertyEdit(const PropertyEdit& other) = delete;
    PropertyEdit(PropertyEdit&& other) = default;
    ~PropertyEdit() = default;

    PropertyEdit& operator = (const PropertyEdit& rhs) = delete;
    PropertyEdit& operator = (PropertyEdit&& rhs) = default;

    std::optional<bool> inputParsable() const;

    void render();

private:

    std::optional<bool> m_inputParsable;
    dots::type::ProxyProperty<> m_property;
    std::string m_buffer;
    std::string m_header;
    std::string m_label;
};
