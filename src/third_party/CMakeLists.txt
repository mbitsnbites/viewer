# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

# We're not interested in warnings for 3rd party libraries.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # Remove any warning flags (avoid MSVC "Command line warning D9025").
  string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

  # Inhibit warnings.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
else()
  # Inhibit warnings (append -w at the end of the compiler flags strings,
  # overriding any previous warning settings).
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(glfw)

# gl3w
add_subdirectory(gl3w)

# ImGui
add_subdirectory(imgui)
