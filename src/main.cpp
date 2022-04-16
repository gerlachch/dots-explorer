#include <backends/GlfwOpenGl3Backend.h>
#include <components/MainWindow.h>
#include <boost/asio.hpp>
#include <dots/Application.h>

int main()
{
    dots::Application app{ "dots-imgui" };
    GlfwOpenGl3Backend backend{ "dots-imgui" };

    dots::publish(DotsDescriptorRequest{});

    backend.run([&]
    {
        app.transceiver().ioContext().poll();
        MainWindow mainWindow;
        mainWindow.render();
    });
}
