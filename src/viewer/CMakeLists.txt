# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(viewer_sources
    main.cc
    main_window.cc
    main_window.h
    main_window_worker.cc
    main_window_worker.h
    viewer.cc
    viewer.h)

find_package(Threads REQUIRED)

add_executable(viewer ${viewer_sources})
target_link_libraries(viewer base ui gl3w glfw ${GLFW_LIBRARIES} imgui ${CMAKE_THREAD_LIBS_INIT})
