gfx_sources = ['shader.cc',
               'shader.h']

gfx_lib = library('gfx',
                  gfx_sources,
                  include_directories: [root_inc],
                  dependencies: [gl3w])

gfx = declare_dependency(link_with: gfx_lib)

