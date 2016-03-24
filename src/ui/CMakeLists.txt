# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(ui_sources
    application.cc
    application.h
    offscreen_context.cc
    offscreen_context.h
    ui_window.cc
    ui_window.h
    window.cc
    window.h)

add_library(ui ${ui_sources})
target_link_libraries(ui base gfx gl3w glfw ${GLFW_LIBRARIES} imgui)
