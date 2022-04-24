#pragma once
#include <string>
#include <optional>
#include <dots/dots.h>
#include <imgui.h>
#include <tools/TypeRandomizer.h>
#include <widgets/PropertyDescription.h>

struct PropertyEdit
{
    PropertyEdit(dots::type::Struct& instance, const dots::type::PropertyPath& propertyPath);
    PropertyEdit(const PropertyEdit& other) = delete;
    PropertyEdit(PropertyEdit&& other) = default;
    ~PropertyEdit() = default;

    PropertyEdit& operator = (const PropertyEdit& rhs) = delete;
    PropertyEdit& operator = (PropertyEdit&& rhs) = default;

    const dots::type::ProxyProperty<>& property() const;
    std::optional<bool> inputParseable() const;

    void render();

private:

    std::string m_inputBuffer;
    std::optional<ImVec4> m_inputColor;
    std::optional<bool> m_inputParseable;
    std::optional<dots::type::TypeRandomizer<>> m_randomizer;
    dots::type::ProxyProperty<> m_property;
    PropertyDescription m_propertyDescription;
    std::string m_inputLabel;
    std::string m_invalidateLabel;
    std::string m_randomizeLabel;
};
