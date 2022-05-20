#pragma once
#include <string>
#include <optional>
#include <dots/dots.h>
#include <models/PropertyModel.h>

struct PropertyEdit
{
    PropertyEdit(PropertyModel& model);
    PropertyEdit(const PropertyEdit& other) = delete;
    PropertyEdit(PropertyEdit&& other) = default;
    ~PropertyEdit() = default;

    PropertyEdit& operator = (const PropertyEdit& rhs) = delete;
    PropertyEdit& operator = (PropertyEdit&& rhs) = default;

    const PropertyModel& model() const;
    PropertyModel& model();

    std::optional<bool> included() const;

    void render();

private:

    using model_ref_t = std::reference_wrapper<PropertyModel>;

    std::string m_inputBuffer;
    std::optional<bool> m_included;
    model_ref_t m_model;
    std::string m_inputLabel;
    std::string m_invalidateLabel;
    std::string m_randomizeLabel;
    std::string m_timepointNowLabel;
};
