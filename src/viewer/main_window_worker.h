// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-
//------------------------------------------------------------------------------
// Copyright (c) 2016, Marcus Geelnard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#ifndef VIEWER_MAIN_WINDOW_WORKER_H_
#define VIEWER_MAIN_WINDOW_WORKER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace ui {

class OffscreenContext;
class Window;

}  // namespace ui

namespace viewer {

/// @brief The main worker.
class MainWindowWorker {
 public:
  /// @brief Constructor.
  /// @param share_window The window that the worker context will share OpenGL
  /// objects with.
  explicit MainWindowWorker(const ui::Window& share_window);

  /// @brief Destructor.
  ///
  /// The destructor terminates the worker thread and blocks until the thread
  /// has terminated gracefully.
  ~MainWindowWorker();

 private:
  void Run();

  std::atomic_bool terminate_thread_;
  std::condition_variable condition_variable_;
  std::mutex mutex_;
  std::thread thread_;

  std::unique_ptr<ui::OffscreenContext> gl_context_;

  // Disable copy/move.
  MainWindowWorker(const MainWindowWorker&) = delete;
  MainWindowWorker(MainWindowWorker&&) = delete;
  MainWindowWorker& operator=(const MainWindowWorker&) = delete;
};

}  // namespace viewer

#endif  // VIEWER_MAIN_WINDOW_WORKER_H_
