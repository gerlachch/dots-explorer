#pragma once
#include <optional>
#include <dots/dots.h>
#include <imgui.h>

struct InstanceView
{
    InstanceView(const dots::type::Struct& instance);
    InstanceView(const InstanceView& other) = delete;
    InstanceView(InstanceView&& other) = default;
    ~InstanceView() = default;

    InstanceView& operator = (const InstanceView& rhs) = delete;
    InstanceView& operator = (InstanceView&& rhs) = default;

    const dots::type::Struct& instance() const;

    void update();
    void render();

private:

    using struct_ref_t = std::reference_wrapper<const dots::type::Struct>;
    struct property_view;

    struct property_view
    {
        std::string value;
        std::optional<ImVec4> color = std::nullopt;
        bool isValid = true;
    };
    
    struct_ref_t m_instance;
    std::vector<property_view> m_propertyViews;
};
