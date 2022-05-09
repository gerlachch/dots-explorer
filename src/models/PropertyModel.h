#pragma once
#include <dots/dots.h>
#include <common/ImGuiExt.h>
#include <dots_ext/TypeRandomizer.h>
#include <models/PropertyDescriptorModel.h>

struct ImGuiTableColumnSortSpecs;

struct PropertyModel
{
    PropertyModel(const PropertyDescriptorModel& descriptorModel, dots::type::Struct& instance);
    PropertyModel(const PropertyDescriptorModel& descriptorModel, const dots::type::Struct& instance);

    const PropertyDescriptorModel& descriptorModel() const;

    const dots::type::ProxyProperty<>& property() const;
    dots::type::ProxyProperty<>& property();

    const std::string& toString() const;
    bool fromString(const std::string& value);

    void invalidate();
    void randomize();

    bool valueChanged() const;
    const ImGuiExt::ColoredText& valueText() const;

    bool less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyModel& other) const;

    void fetch();

private:

    using descriptor_model_ref_t = std::reference_wrapper<const PropertyDescriptorModel>;

    inline static dots::type::TypeRandomizer<> M_randomizer{ std::random_device{}() };

    mutable ImGuiExt::ColoredText m_valueText;
    bool m_mutable;
    descriptor_model_ref_t m_descriptorModel;
    dots::type::ProxyProperty<> m_property;
};
