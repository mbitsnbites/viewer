# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(imgui_sources
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h)

add_library(imgui ${imgui_sources})
target_include_directories(imgui PUBLIC .)
