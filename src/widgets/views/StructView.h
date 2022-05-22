#pragma once
#include <dots/dots.h>
#include <models/MetadataModel.h>
#include <models/StructRefModel.h>

struct ImGuiTableSortSpecs;

struct StructView
{
    StructView(const MetadataModel& metadataModel, const StructRefModel& structRefModel);
    StructView(const StructView& other) = delete;
    StructView(StructView&& other) = default;
    ~StructView() = default;

    StructView& operator = (const StructView& rhs) = delete;
    StructView& operator = (StructView&& rhs) = default;

    const MetadataModel& metadataModel() const;
    const StructRefModel& structRefModel() const;

    void render();

private:

    using metadata_model_ref_t = std::reference_wrapper<const MetadataModel>;
    using struct_model_ref_t = std::reference_wrapper<const StructRefModel>;

    metadata_model_ref_t m_metadataModel;
    struct_model_ref_t m_structRefModel;
};
