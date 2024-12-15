#pragma once
#include <vector>
#include <filesystem>
#include <dots/Connection.h>
#include <dots/type/Registry.h>
#include <dots/io/channels/LocalChannel.h>
#include <dots/serialization/CborSerializer.h>

namespace dots::io::details
{
    struct GenericFileInChannel : LocalChannel
    {
        GenericFileInChannel(key_t key, asio::io_context& ioContext, std::filesystem::path path, const std::vector<type::Descriptor<>*> &preregister_descriptors = {});
        GenericFileInChannel(const GenericFileInChannel& other) = delete;
        GenericFileInChannel(GenericFileInChannel&& other) = delete;
        ~GenericFileInChannel() override = default;

        GenericFileInChannel& operator = (const GenericFileInChannel& rhs) = delete;
        GenericFileInChannel& operator = (GenericFileInChannel&& rhs) = delete;

    protected:

        void asyncReceiveImpl() override;

    private:

        bool handleTransmission(Connection& connection, Transmission transmission);
        void handleTransition(Connection& connection, std::exception_ptr ePtr) noexcept;

        void loadFile();

        std::vector<uint8_t> m_buffer;
        serialization::CborSerializer m_serializer;

        std::optional<Connection> m_fileConnection;
        type::Registry m_fileRegistry;

        size_t m_transmissionsRead;
        std::reference_wrapper<asio::io_context> m_ioContext;
        std::filesystem::path m_path;
    };
}

namespace dots::io
{
    inline namespace v1
    {
        using FileInChannel = details::GenericFileInChannel;
    }
}
