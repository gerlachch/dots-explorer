#pragma once
#include <filesystem>
#include <fstream>
#include <dots/Connection.h>
#include <dots/type/Registry.h>
#include <dots/io/channels/LocalChannel.h>
#include <dots/serialization/CborSerializer.h>

namespace dots::io::details
{
    struct GenericFileOutChannel : Channel
    {
        GenericFileOutChannel(key_t key, asio::io_context& ioContext, std::filesystem::path path);
        GenericFileOutChannel(const GenericFileOutChannel& other) = delete;
        GenericFileOutChannel(GenericFileOutChannel&& other) = delete;
        ~GenericFileOutChannel() override = default;

        GenericFileOutChannel& operator = (const GenericFileOutChannel& rhs) = delete;
        GenericFileOutChannel& operator = (GenericFileOutChannel&& rhs) = delete;

    protected:

        void asyncReceiveImpl() override;
        void transmitImpl(const DotsHeader& header, const type::Struct& instance) override;

    private:
        
        serialization::CborSerializer m_serializer;
        std::ofstream m_file;
        std::reference_wrapper<asio::io_context> m_ioContext;
        std::filesystem::path m_path;
    };
}

namespace dots::io
{
    inline namespace v1
    {
        using FileOutChannel = details::GenericFileOutChannel;
    }
}
