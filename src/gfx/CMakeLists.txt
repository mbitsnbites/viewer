# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(gfx_sources
    shader.cc
    shader.h)

add_library(gfx ${gfx_sources})
target_link_libraries(gfx base gl3w)
