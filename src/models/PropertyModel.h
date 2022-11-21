#pragma once
#include <dots/dots.h>
#include <common/ImGuiExt.h>
#include <dots_ext/TypeRandomizer.h>
#include <models/PropertyDescriptorModel.h>

struct ImGuiTableColumnSortSpecs;

struct PropertyModel
{
    PropertyModel(PropertyDescriptorModel descriptorModel, dots::type::Struct& instance);
    PropertyModel(PropertyDescriptorModel descriptorModel, const dots::type::Struct& instance);

    const PropertyDescriptorModel& descriptorModel() const;

    const dots::type::ProxyProperty<>& property() const;
    dots::type::ProxyProperty<>& property();

    const std::string& toString() const;
    bool fromString(const std::string& value);

    void invalidate();
    void randomize();

    bool valueChanged() const;
    ImGuiExt::ColoredTextView valueText() const;

    bool less(const ImGuiTableColumnSortSpecs& sortSpec, const PropertyModel& other) const;

    void fetch();

private:

    inline static dots::type::TypeRandomizer<> M_randomizer{ std::random_device{}() };

    mutable std::string m_valueStr;
    bool m_mutable;
    PropertyDescriptorModel m_descriptorModel;
    dots::type::ProxyProperty<> m_property;
};
