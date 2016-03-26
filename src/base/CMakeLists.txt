# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(base_sources
    error.cc
    error.h
    make_unique.h)

add_library(base ${base_sources})
