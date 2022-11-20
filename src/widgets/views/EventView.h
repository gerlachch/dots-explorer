#pragma once
#include <dots/dots.h>
#include <models/EventModel.h>

struct EventView
{
    EventView(EventModel model);

    void render() const;

private:

    EventModel m_model;
};
