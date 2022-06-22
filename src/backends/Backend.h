#pragma once
#include <string_view>
#include <atomic>
#include <functional>

struct Backend
{
    Backend(int width, int height, std::string_view title);
    Backend(const Backend& other) = default;
    Backend(Backend&& other) = default;
    ~Backend();

    Backend& operator = (const Backend& rhs) = default;
    Backend& operator = (Backend&& rhs) = default;

    void run(std::function<void()> renderHandler);

private:

    inline static std::atomic_flag M_initialized = ATOMIC_FLAG_INIT;
};
