#pragma once
#include <string_view>
#include <atomic>
#include <functional>

struct GlfwBackend
{
    GlfwBackend(std::string_view title);
    GlfwBackend(const GlfwBackend& other) = default;
    GlfwBackend(GlfwBackend&& other) = default;
    ~GlfwBackend();

    GlfwBackend& operator = (const GlfwBackend& rhs) = default;
    GlfwBackend& operator = (GlfwBackend&& rhs) = default;

    void run(std::function<void()> renderHandler);

private:

    inline static std::atomic_flag M_initialized = ATOMIC_FLAG_INIT;
};
