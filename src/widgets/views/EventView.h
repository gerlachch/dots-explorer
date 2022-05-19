#pragma once
#include <dots/dots.h>
#include <models/MetadataModel.h>
#include <models/StructRefModel.h>

struct EventView
{
    EventView(const MetadataModel& metadataModel, const StructRefModel& publishedInstanceModel, const StructRefModel& updatedInstanceModel);

    void render() const;

private:

    using metadata_model_ref_t = std::reference_wrapper<const MetadataModel>;
    using struct_model_ref_t = std::reference_wrapper<const StructRefModel>;

    metadata_model_ref_t m_metadataModel;
    struct_model_ref_t m_publishedInstanceModel;
    struct_model_ref_t m_updatedInstanceModel;
};
