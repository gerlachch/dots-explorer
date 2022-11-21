#pragma once
#include <vector>
#include <dots/dots.h>
#include <models/StructModel.h>
#include <widgets/dialogs/PropertyEdit.h>

struct PublishDialog
{
    PublishDialog(StructModel structModel);
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
    StructModel m_structModel;
    std::vector<PropertyEdit> m_propertyEdits;
};
