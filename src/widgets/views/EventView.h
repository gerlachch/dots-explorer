#pragma once
#include <dots/dots.h>
#include <models/EventModel.h>

struct EventView
{
    EventView(std::shared_ptr<const EventModel> model);

    void render() const;

private:

    using metadata_model_ref_t = std::reference_wrapper<const MetadataModel>;
    using struct_model_ref_t = std::reference_wrapper<const StructModel>;

    std::shared_ptr<const EventModel> m_model;
};
