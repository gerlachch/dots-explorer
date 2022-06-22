# dots-explorer

The *dots-explorer* is a utility application for the DOTS IPC system (see [dots-cpp](https://github.com/pnxs/dots-cpp)).

# TL;DR

1. Download latest [official release](https://github.com/gerlachch/dots-explorer/releases).
2. Unpack to suitable directory.
3. Launch without arguments.

# Overview

The dots-explorer can be used to inspect and manipulate objects of a DOTS environment.

## Features

* Dynamically navigate and inspect type caches.
* Filter and sort instances and types.
* Easily publish and remove instances.
* Fast and bloat-free based on [Dear ImGui](https://github.com/ocornut/imgui).
* Simple to install.
* Portable with officially maintained support for Linux (with GCC 9.4.0) and Windows (with latest MSVC).

# Runtime Dependencies

* Official releases require DirectX 11 on Windows and the OpenGL runtime on Linux, both of which should already be installed by default on most systems. Alternatively, the project can also target Vulkan (see below).
* (Optional) When curl is available, it will be used to fetch release information from the GitHub repository.

# Build Dependencies

* All dependencies of [dots-cpp](https://github.com/pnxs/dots-cpp#dependencies) (but not dots-cpp itself!).
* Boost Filesystem (required by Boost Process).
* Windows SDK (Windows builds), OpenGL SDK (Linux builds) or the Vulkan SDK (when targeting Vulkan).
* C++ compiler supporting at least C++17 (such as GCC 9 or MSVC 19.14).

# Build and Run

This is how the dots-explorer can be built and run based on the example of Ubuntu 20.04. Note that the following steps presume that the environment already fulfills the requirements for [dots-cpp](https://github.com/pnxs/dots-cpp#build-and-run).

Install build dependencies (includes the optional [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#linux)):

```sh
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add - && \
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-focal.list https://packages.lunarg.com/vulkan/lunarg-vulkan-focal.list && \
sudo apt-get update -yq && \
sudo apt-get install -yq \
    vulkan-sdk \
    libboost-filesystem-dev=1.71.0.0ubuntu2 \
    xorg-dev
```

Clone repository including submodules:

```sh
git clone --recurse-submodules https://github.com/gerlachch/dots-explorer.git && cd dots-explorer
```

Build (OpenGL):

```sh
cmake -G Ninja -B ./build/ && cmake --build ./build/
```

Build (Vulkan):

```sh
cmake -G Ninja -B ./build/ -DDOTS_EXPLORER_USE_VULKAN_BACKEND=ON && cmake --build ./build/
```

Run:

```sh
./build/dots-explorer
```

# License

This project is currently licensed under the GPL-3.0.
