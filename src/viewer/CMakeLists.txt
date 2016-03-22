# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(viewer_sources
    error.h
    main.cc
    main_window.cc
    main_window.h
    main_window_worker.cc
    main_window_worker.h
    viewer.cc
    viewer.h)

set(viewer_sources ${viewer_sources}
    graphics/shader.cc
    graphics/shader.h)

set(viewer_sources ${viewer_sources}
    utils/make_unique.h)

add_executable(viewer ${viewer_sources})
target_link_libraries(viewer ui gl3w glfw ${GLFW_LIBRARIES} imgui)
