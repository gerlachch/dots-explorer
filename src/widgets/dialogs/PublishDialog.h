#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructRefModel.h>
#include <widgets/dialogs/PropertyEdit.h>

struct PublishDialog
{
    PublishDialog(const StructDescriptorModel& structDescriptorModel, dots::type::AnyStruct instance);
    PublishDialog(const PublishDialog& other) = delete;
    PublishDialog(PublishDialog&& other) = default;
    ~PublishDialog() = default;

    PublishDialog& operator = (const PublishDialog& rhs) = delete;
    PublishDialog& operator = (PublishDialog&& rhs) = default;

    bool render();

private:

    inline static uint64_t M_id;

    std::string m_popupId;
    bool m_remove;
    dots::type::AnyStruct m_instance;
    StructRefModel m_structRefModel;
    std::vector<PropertyEdit> m_propertyEdits;
};
