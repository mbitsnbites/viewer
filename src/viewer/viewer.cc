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
#include "imgui/imgui.h"

#include "viewer/error.h"
#include "viewer/utils/make_unique.h"

namespace viewer {

namespace {

/// @brief The application main window.
class MainWindow : public UiWindow {
 public:
  MainWindow() : UiWindow(1024, 576, "Viewer") {}

 private:
  void DefineUi() override {
    // 1. Show the main window.
    if (show_main_window_) {
      ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Main Window", &show_main_window_);
      ImGui::Text("Hello, world!");
      ImGui::SliderFloat("Some float", &float_value_, 0.0f, 1.0f);
      ImGui::ColorEdit3("Some color",
                        reinterpret_cast<float*>(&color_value_));
      if (ImGui::Button("Another Window")) {
        show_another_window_ ^= 1;
      }
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate,
                  ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // 2. Show another simple window.
    if (show_another_window_) {
      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Another Window", &show_another_window_);
      ImGui::Text("Hello");
      ImGui::End();
    }
  }

  ImVec4 color_value_ = ImColor(114, 144, 154);
  float float_value_ = 0.5f;
  bool show_main_window_ = true;
  bool show_another_window_ = false;
};

}  // namespace

void Viewer::Run() {
  // Create the main window.
  main_window_ = make_unique<MainWindow>();

  // Main loop.
  while (!main_window_->ShouldClose()) {
    glfwPollEvents();

    // Activate the main window for painting.
    main_window_->BeginFrame();

    // Clear the screen.
    glClearColor(1.0f, 0.6f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO(m): Paint the 3D world.

    // Paint the UI.
    main_window_->PaintUi();

    main_window_->SwapBuffers();
  }
}

Viewer::GlfwContext::GlfwContext() {
  if (glfwInit() == GL_FALSE) {
    throw Error("Unable to initialize GLFW.");
  }
}

Viewer::GlfwContext::~GlfwContext() {
  glfwTerminate();
}

}  // namespace viewer
