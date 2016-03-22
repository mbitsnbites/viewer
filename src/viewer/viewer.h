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

#ifndef VIEWER_VIEWER_H_
#define VIEWER_VIEWER_H_

#include <memory>

#include "viewer/ui/ui_window.h"

namespace viewer {

/// @brief The viewer application instance.
class Viewer {
 public:
  /// @brief Run the application.
  /// @note This method blocks until the application terminates.
  void Run();

 private:
  /// @brief A class for handling GLFW initialization and termination.
  class GlfwContext {
   public:
    /// @brief Initialize GLFW.
    GlfwContext();

    /// @brief Terminate GLFW.
    ~GlfwContext();
  };

  // Note: The GLFW context is initialized before any GLFW windows are created,
  // and destroyed after all GLFW windows have been destroyed (i.e. member order
  // is important).
  GlfwContext glfw_context_;

  std::unique_ptr<UiWindow> main_window_;
};

}  // namespace viewer

#endif  // VIEWER_VIEWER_H_
