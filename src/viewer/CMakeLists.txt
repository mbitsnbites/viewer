# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(viewer_sources
    error.h
    main.cc
    viewer.cc
    viewer.h)

set(viewer_sources ${viewer_sources}
    ui/ui.cc
    ui/ui.h)

add_executable(viewer ${viewer_sources})
target_link_libraries(viewer gl3w glfw ${GLFW_LIBRARIES} imgui)
