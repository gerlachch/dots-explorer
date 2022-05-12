#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructModel.h>
#include <widgets/dialogs/PropertyEdit.h>

struct StructEdit
{
    StructEdit(const StructDescriptorModel& structDescriptorModel, dots::type::AnyStruct instance);
    StructEdit(const StructEdit& other) = delete;
    StructEdit(StructEdit&& other) = default;
    ~StructEdit() = default;

    StructEdit& operator = (const StructEdit& rhs) = delete;
    StructEdit& operator = (StructEdit&& rhs) = default;

    bool render();

private:

    inline static uint64_t M_id;

    std::string m_popupId;
    dots::type::AnyStruct m_instance;
    StructModel m_structModel;
    std::vector<PropertyEdit> m_propertyEdits;
};
