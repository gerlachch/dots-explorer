#include <backends/GlfwBackend.h>
#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <dots/Application.h>

int main()
{
    dots::Application app{ "dots-imgui" };
    GlfwBackend backend{ 1280, 720, "dots-imgui" };

    dots::publish(DotsDescriptorRequest{});

    backend.run([&]
    {
        app.transceiver().ioContext().poll();
        MainWindow mainWindow;
        mainWindow.render();
    });
}
