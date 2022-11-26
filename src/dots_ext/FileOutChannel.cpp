#include <dots_ext/FileOutChannel.h>
#include <dots/io/DescriptorConverter.h>
#include <dots/asio.h>
#include <dots/HostTransceiver.h>

namespace dots::io::details
{
    GenericFileOutChannel::GenericFileOutChannel(key_t key, asio::io_context& ioContext, std::filesystem::path path) :
        Channel(key),
        m_ioContext{ ioContext },
        m_path{ std::move(path) }
    {
        m_file.exceptions(std::iostream::failbit | std::iostream::badbit);
        m_file.open(m_path.string(), std::ios::binary);
    }

    void GenericFileOutChannel::asyncReceiveImpl()
    {
        /* do nothing */
    }

    void GenericFileOutChannel::transmitImpl(const DotsHeader& header, const type::Struct& instance)
    {
        m_serializer.serialize(header);
        m_serializer.serialize(instance);

        auto& output = m_serializer.output();
        m_file.write(reinterpret_cast<char*>(output.data()), static_cast<std::streamsize>(output.size()));
        output.clear();
    }
}
