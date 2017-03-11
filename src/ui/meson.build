ui_sources = ['application.cc',
              'application.h',
              'offscreen_context.cc',
              'offscreen_context.h',
              'ui_window.cc',
              'ui_window.h',
              'window.cc',
              'window.h']

ui_lib = library('ui',
                 ui_sources,
                 include_directories: [root_inc],
                 dependencies: [base, gfx, glfw, gl3w, imgui])

ui = declare_dependency(link_with: ui_lib)

