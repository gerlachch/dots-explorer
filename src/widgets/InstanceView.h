#pragma once
#include <optional>
#include <dots/dots.h>
#include <widgets/PropertyView.h>

struct ImGuiTableSortSpecs;

struct InstanceView
{
    InstanceView(const dots::type::Struct& instance);
    InstanceView(const InstanceView& other) = delete;
    InstanceView(InstanceView&& other) = default;
    ~InstanceView() = default;

    InstanceView& operator = (const InstanceView& rhs) = delete;
    InstanceView& operator = (InstanceView&& rhs) = default;

    const dots::type::Struct& instance() const;
    bool less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const;

    void update();
    void render();

private:

    using struct_ref_t = std::reference_wrapper<const dots::type::Struct>;
    
    struct_ref_t m_instance;
    std::vector<PropertyView> m_propertyViews;
};
