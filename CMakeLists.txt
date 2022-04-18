cmake_minimum_required(VERSION 3.12)
project(dots-explorer LANGUAGES CXX)
set(TARGET_NAME dots-explorer)

# includes
include(FetchContent)

# CMake options
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    # use default build type if none was specified
    set(DEFAULT_BUILD_TYPE "Release")
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
set(Boost_USE_STATIC_LIBS ON)
if (MSVC)
    # use standard C++ exception model and assume no exceptions for external C functions
    # note that this needs to be set globally to include external targets because MSVC is disadvising
    # from mixing exceptions modes
    add_compile_options(/EHsc)
    # replace default warning level that CMake sets when using MSVC. the override code is based
    # on an example given by Kitware in [1]. note that this issue should already have been fixed since
    # CMake 3.15 (see [2]), but for some reason still occurs with the CMake 3.19.20122902-MSVC_2 version
    # used by Visual Studio 16.9.3
    #
    # References:
    #
    #  - [1] https://gitlab.kitware.com/cmake/community/wikis/FAQ#dynamic-replace
    #
    #  - [2] https://gitlab.kitware.com/cmake/cmake/-/issues/19084
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

# dependencies
find_package(Vulkan)
if (NOT Vulkan_FOUND)
    message("Vulkan SDK not found. Attempting to fall back to OpenGL instead.")
    find_package(OpenGL REQUIRED)
endif()

option(DOTS_BUILD_EXAMPLES "Build the examples" OFF)
option(DOTS_BUILD_UNIT_TESTS "Build the unit tests" OFF)
FetchContent_Declare(
    dots-cpp
    GIT_REPOSITORY https://github.com/pnxs/dots-cpp.git
    GIT_TAG        v1.6.2
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        8.0.1
)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(dots-cpp fmt glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        master
)
# Check if population has already been performed
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
  FetchContent_Populate(imgui)
endif()

# target
add_executable(${TARGET_NAME})

# properties
target_sources(${TARGET_NAME}
    PRIVATE
        src/main.cpp
        $<$<BOOL:${Vulkan_FOUND}>:src/backends/GlfwVulkanBackend.cpp>
        $<$<BOOL:${OpenGL_FOUND}>:src/backends/GlfwOpenGl3Backend.cpp>
        src/components/MainWindow.cpp
        src/widgets/ContainerView.cpp
        src/widgets/InstanceEdit.cpp
        src/widgets/InstanceView.cpp
        src/widgets/PoolView.cpp
        src/widgets/PropertyView.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        $<$<BOOL:${Vulkan_FOUND}>:${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp>
        $<$<BOOL:${OpenGL_FOUND}>:${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp>
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_compile_options(${TARGET_NAME}
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<BOOL:${CMAKE_CXX_FLAGS}>>>:-Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
target_compile_definitions(${TARGET_NAME}
    PRIVATE
        # suppress warning for usage of unsafe C runtime functions (e.g. getenv)
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<BOOL:${Vulkan_FOUND}>:ImTextureID=ImU64>
        $<$<BOOL:${Vulkan_FOUND}>:VK_PROTOTYPES>
)
target_compile_features(${TARGET_NAME}
    PRIVATE
        cxx_std_17
)
target_link_libraries(${TARGET_NAME}
    PRIVATE
        DOTS::DOTS
        fmt::fmt
        glfw
        $<$<BOOL:${Vulkan_FOUND}>:Vulkan::Vulkan>
        $<$<BOOL:${OpenGL_FOUND}>:${OPENGL_LIBRARIES}>
)

# install
install (TARGETS ${TARGET_NAME} DESTINATION sbin)
