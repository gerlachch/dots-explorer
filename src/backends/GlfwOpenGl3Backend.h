#pragma once
#include <string_view>
#include <atomic>
#include <functional>

struct GlfwOpenGl3Backend
{
    GlfwOpenGl3Backend(std::string_view title);
    GlfwOpenGl3Backend(const GlfwOpenGl3Backend& other) = default;
    GlfwOpenGl3Backend(GlfwOpenGl3Backend&& other) = default;
    ~GlfwOpenGl3Backend();

    GlfwOpenGl3Backend& operator = (const GlfwOpenGl3Backend& rhs) = default;
    GlfwOpenGl3Backend& operator = (GlfwOpenGl3Backend&& rhs) = default;

    void run(std::function<void()> renderHandler);

private:

    inline static std::atomic_flag M_initialized = ATOMIC_FLAG_INIT;
};
