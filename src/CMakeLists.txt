# -*- mode: CMake; tab-width: 2; indent-tabs-mode: nil; -*-

cmake_minimum_required(VERSION 2.8.12)

project(viewer)

# Extra compiler flags.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  set(compiler_flags "/MP /WX")
else()
  set(compiler_flags "-W -Wall -Wextra -pedantic -Werror")
  set(cxx_flags "-std=c++11")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags} ${cxx_flags}")

if(APPLE)
  # Use libc++ on Mac OS X.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

if(UNIX AND NOT APPLE)
  # Use ld.gold on Linux.
  execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
  if("${ld_version}" MATCHES "GNU gold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
  endif()
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  # Make sure that we find <GL/*.h> and friends on FreeBSD systems.
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

# Add third party libraries.
add_subdirectory(third_party)

# All includes are relative to the root directory.
get_filename_component(root "." ABSOLUTE)
include_directories(${root})

# Add project internal libraries.
add_subdirectory(base)
add_subdirectory(gfx)
add_subdirectory(ui)

# Add viewer porject(s).
add_subdirectory(viewer)

