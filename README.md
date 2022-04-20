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

* Official releases require the Vulkan runtime. If not available, the project can also be built to target OpenGL (see below).

# Build Dependencies

* All dependencies of [dots-cpp](https://github.com/pnxs/dots-cpp#dependencies) (but not dots-cpp itself!).
* Either the Vulkan or OpenGL SDK (with the former being preferred when available).
* C++ compiler supporting at least C++17 (such as GCC 9 or MSVC 19.14).

# Build and Run

This is how the dots-explorer can be built and run based on the example of Ubuntu 20.04. Note that the following steps presume that the environment already fulfills the requirements for [dots-cpp](https://github.com/pnxs/dots-cpp#build-and-run).

Install build dependencies including Vulkan SDK (see [here](https://vulkan.lunarg.com/sdk/home#linux)):

```sh
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add - && \
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-focal.list https://packages.lunarg.com/vulkan/lunarg-vulkan-focal.list && \
sudo apt-get update -yq && \
sudo apt-get install -yq \
    vulkan-sdk \
    xorg-dev
```

Clone repository including submodules:

```sh
git clone --recurse-submodules https://github.com/gerlachch/dots-explorer.git && cd dots-explorer
```

Build:

```sh
cmake -G Ninja -B ./build/ && cmake --build ./build/
```

Run:

```sh
./build/dots-explorer
```

# License

This project is currently licensed under the GPL-3.0.
