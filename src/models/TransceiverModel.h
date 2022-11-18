#pragma once
#include <vector>
#include <deque>
#include <dots/dots.h>
#include <models/EventModel.h>

struct TransceiverModel
{
    using event_handler_t = dots::tools::Handler<void(const event_model_ptr_t&)>;
    using struct_type_handler_t = dots::tools::Handler<void(const StructDescriptorModel&)>;

    TransceiverModel();
    TransceiverModel(const TransceiverModel& other) = delete;
    TransceiverModel(TransceiverModel&& other) = default;
    ~TransceiverModel() = default;

    TransceiverModel& operator = (const TransceiverModel& rhs) = delete;
    TransceiverModel& operator = (TransceiverModel&& rhs) = default;
    
    void subscribe(const StructDescriptorModel& descriptorModel, event_handler_t handler);
    void subscribe(event_handler_t handler);
    void subscribe(struct_type_handler_t handler);

private:

    void update(const dots::type::StructDescriptor& descriptor);
    
    std::vector<dots::Subscription> m_subscriptions;
    std::deque<StructDescriptorModel> m_descriptorModels;
    std::unordered_map<const StructDescriptorModel*, std::vector<event_handler_t>> m_eventHandlersTyped;
    std::vector<event_handler_t> m_eventHandlers;
    std::vector<struct_type_handler_t> m_structTypeHandlers;
    PublisherModel m_publisherModel;
    size_t m_eventIndex;
};
