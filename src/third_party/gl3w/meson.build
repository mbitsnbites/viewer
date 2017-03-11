gl3w_sources = ['include/GL/gl3w.h',
                'include/GL/glcorearb.h',
                'src/gl3w.c']

gl3w_inc = include_directories('include')

# We need OpenGL.
gl_dep = dependency('gl', required: true)

# For platforms where it is available, we need dl (dlopen & friends).
dl_dep = meson.get_compiler('c').find_library('dl', required: false)

gl3w_lib = library('gl3w',
                   gl3w_sources,
                   include_directories: gl3w_inc,
                   dependencies: [gl_dep, dl_dep])

gl3w = declare_dependency(link_with: gl3w_lib,
                          include_directories: gl3w_inc)

