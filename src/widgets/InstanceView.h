#pragma once
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

    const char* widgetId() const;
    DotsMt lastOperation() const;
    const dots::type::Struct& instance() const;

    bool less(const ImGuiTableSortSpecs& sortSpecs, const InstanceView& other) const;
    bool isSelected() const;

    void update(const dots::Event<>& event);
    void render();

private:

    using struct_ref_t = std::reference_wrapper<const dots::type::Struct>;

    inline static uint64_t M_nextWidgetId = 0;

    std::string m_widgetId;
    DotsMt m_lastOperation;
    struct_ref_t m_instance;
    std::vector<PropertyView> m_propertyViews;
};
