#pragma once
#include <dots/dots.h>
#include <models/PropertyModel.h>

struct PropertyView
{
    PropertyView(PropertyModel& model);
    PropertyView(const PropertyView& other) = delete;
    PropertyView(PropertyView&& other) = default;
    ~PropertyView() = default;

    PropertyView& operator = (const PropertyView& rhs) = delete;
    PropertyView& operator = (PropertyView&& rhs) = default;

    const PropertyModel& model() const;
    PropertyModel& model();

    bool isSelected() const;
    void render();

private:

    using model_ref_t = std::reference_wrapper<PropertyModel>;

    bool m_isSelected;
    model_ref_t m_model;
};
