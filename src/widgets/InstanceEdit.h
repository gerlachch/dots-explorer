#pragma once
#include <vector>
#include <dots/dots.h>
#include <widgets/StructDescription.h>
#include <widgets/PropertyEdit.h>

struct InstanceEdit
{
    InstanceEdit(dots::type::AnyStruct instance);
    InstanceEdit(const InstanceEdit& other) = delete;
    InstanceEdit(InstanceEdit&& other) = default;
    ~InstanceEdit() = default;

    InstanceEdit& operator = (const InstanceEdit& rhs) = delete;
    InstanceEdit& operator = (InstanceEdit&& rhs) = default;
    
    bool render(const StructDescription& structDescription, const std::vector<PropertyDescription>& propertyDescriptions);

private:

    inline static uint64_t M_id;

    std::string m_popupId;
    dots::type::AnyStruct m_instance;
    std::vector<PropertyEdit> m_propertyEdits;
};
