#pragma once
#include <dots/dots.h>
#include <models/PropertyModel.h>

struct StructListColumn
{
    StructListColumn(PropertyModel& model);
    StructListColumn(const StructListColumn& other) = delete;
    StructListColumn(StructListColumn&& other) = default;
    ~StructListColumn() = default;

    StructListColumn& operator = (const StructListColumn& rhs) = delete;
    StructListColumn& operator = (StructListColumn&& rhs) = default;

    const PropertyModel& model() const;
    PropertyModel& model();

    bool isSelected() const;
    void render();

private:

    using model_ref_t = std::reference_wrapper<PropertyModel>;

    bool m_isSelected;
    model_ref_t m_model;
};
