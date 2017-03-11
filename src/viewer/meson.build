viewer_sources = ['main.cc',
                  'main_window.cc',
                  'main_window.h',
                  'main_window_worker.cc',
                  'main_window_worker.h',
                  'viewer.cc',
                  'viewer.h']

thread_dep = dependency('threads', required: true)

viewer = executable('viewer',
                    viewer_sources,
                    include_directories: [root_inc],
                    dependencies: [base, ui, thread_dep, gl3w, imgui])

