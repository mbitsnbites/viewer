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

#ifndef VIEWER_UI_OFFSCREEN_CONTEXT_H_
#define VIEWER_UI_OFFSCREEN_CONTEXT_H_

struct GLFWwindow;

namespace viewer {

class Window;

/// @brief An offscreen OpenGL context.
///
/// The offscreen context is actually implemented as an invisible GLFW window.
class OffscreenContext {
 public:
  /// @brief Construct a new offscreen context.
  /// @param share_window The window with which to share OpenGL objects.
  /// @note GLFW must have been initialized before calling the constructor.
  explicit OffscreenContext(const Window& share_window);

  /// @brief Destroy the offscreen context.
  /// @note GLFW must still be initialzied when destroying the context.
  virtual ~OffscreenContext();

  /// @brief Make the OpenGL context current in the calling thread.
  void MakeCurrent();

  /// @brief Make no OpenGL context current in the calling thread.
  void Release();

 protected:
  GLFWwindow* glfw_window_ = nullptr;

 private:
  // Disable copy/move.
  OffscreenContext(const OffscreenContext&) = delete;
  OffscreenContext(OffscreenContext&&) = delete;
  OffscreenContext& operator=(const OffscreenContext&) = delete;
};

}  // namespace viewer

#endif  // VIEWER_UI_OFFSCREEN_CONTEXT_H_
