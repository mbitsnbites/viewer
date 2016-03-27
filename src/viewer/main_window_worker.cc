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

#include "viewer/main_window_worker.h"

#include <iostream>

#include "base/make_unique.h"
#include "ui/offscreen_context.h"

namespace viewer {

MainWindowWorker::MainWindowWorker(const ui::Window& share_window)
    : terminate_thread_(false) {
  // Create a new off screen OpenGL context.
  gl_context_ = base::make_unique<ui::OffscreenContext>(share_window);
  gl_context_->Release();

  // Start the worker thread.
  thread_ = std::thread(&MainWindowWorker::Run, this);
}

MainWindowWorker::~MainWindowWorker() {
  // Terminate the worker thread.
  terminate_thread_ = true;
  condition_variable_.notify_all();
  thread_.join();
}

void MainWindowWorker::SetFramebufferSize(int width, int height) {
  AppendFunctionCallToQueue(std::bind(&MainWindowWorker::SetFramebufferSizeImpl,
                                      this, width, height));
}

void MainWindowWorker::Run() {
  std::cout << "Started the main worker thread." << std::endl;

  // Activate the off screen OpenGL context.
  gl_context_->MakeCurrent();

  // Main loop.
  while (true) {
    // Wait for a signal.
    std::unique_lock<std::mutex> lock(mutex_);
    while (call_queue_.empty() && !terminate_thread_) {
      condition_variable_.wait(lock);
    }

    // Were we requested to terminate?
    if (terminate_thread_) {
      break;
    }

    // Call a method that was posted on the call queue.
    if (!call_queue_.empty()) {
      // Pop the function call from the queue.
      auto fun = call_queue_.front();
      call_queue_.pop();

      // Unlock the mutex and call the function.
      lock.unlock();
      fun();
    }

    // TODO(m): Repaint the scene if necessary.
  }

  // Release the off screen OpenGL context.
  gl_context_->Release();

  std::cout << "Exiting the main worker thread." << std::endl;
}

void MainWindowWorker::AppendFunctionCallToQueue(
    const std::function<void()>& fun) {
  std::unique_lock<std::mutex> lock(mutex_);
  call_queue_.push(fun);
  condition_variable_.notify_all();
}

void MainWindowWorker::SetFramebufferSizeImpl(int width, int height) {
  // TODO(m): Update the worker framebuffer size.
  std::cout << "SetFramebufferSizeImpl(" << width << "," << height << ");"
            << std::endl;
}

}  // namespace viewer
