imgui_sources = ['imgui/imconfig.h',
                 'imgui/imgui.cpp',
                 'imgui/imgui.h',
                 'imgui/imgui_draw.cpp',
                 'imgui/imgui_internal.h']

imgui_inc = include_directories('.')

imgui_lib = library('imgui',
                    imgui_sources)

imgui = declare_dependency(link_with: imgui_lib,
                           include_directories: imgui_inc)

