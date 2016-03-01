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

#include "viewer/viewer.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "viewer/error.h"

namespace viewer {

namespace {

// Startup window properties.
const int kWinWidth = 1024;
const int kWinHeight = 576;
const char* kWinTitle = "Viewer";

}  // namespace

Viewer::Viewer() {
  if (!glfwInit()) {
    throw Error("Unable to initialize GLFW.");
  }
}

Viewer::~Viewer() {
  glfwTerminate();
}

void Viewer::CreateWindow() {
  // Create a window.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  window_ =
      glfwCreateWindow(kWinWidth, kWinHeight, kWinTitle, nullptr, nullptr);
  if (!window_) {
    throw Error("Unable to open the GLFW window.");
  }

  // Initialize the OpenGL context.
  glfwMakeContextCurrent(window_);
  if (gl3wInit() || !gl3wIsSupported(3, 2)) {
    throw Error("Unable to create an OpenGL 3.2 context.");
  }
}

void Viewer::Run() {
  // Create the main window.
  CreateWindow();

  // Create the UI.
  ui_.reset(new Ui(window_));

  // Main loop.
  while (!glfwWindowShouldClose(window_)) {
    {
      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
    }

    // Clear the screen.
    glClearColor(1.0f, 0.6f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO(m): Paint the 3D world.

    // Paint the UI.
    ui_->Paint();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

}  // namespace viewer
