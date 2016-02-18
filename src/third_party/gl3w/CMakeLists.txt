# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

set(gl3w_sources
    include/GL/gl3w.h
    include/GL/glcorearb.h
    src/gl3w.c)

add_library(gl3w ${gl3w_sources})
target_include_directories(gl3w PUBLIC include)
if(UNIX AND NOT APPLE)
  target_link_libraries(gl3w GL)
elseif(WIN32)
  target_link_libraries(gl3w opengl32)
endif()
