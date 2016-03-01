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

#ifndef VIEWER_VIEWER_UI_UI_H_
#define VIEWER_VIEWER_UI_UI_H_

#include "imgui/imgui.h"

#include "viewer/graphics/shader.h"

struct GLFWwindow;

namespace viewer {

/// @brief The application UI.
class Ui {
 public:
  Ui(GLFWwindow* window);

  /// @brief Paint the UI to the OpenGL context.
  void Paint();

 private:
  bool Init();
  void Shutdown();
  void NewFrame();

  // Use if you want to reset your rendering device without losing ImGui state.
  void InvalidateDeviceObjects();
  bool CreateDeviceObjects();
  bool CreateFontsTexture();

  static void RenderDrawListsBridge(ImDrawData* draw_data);
  void RenderDrawLists(ImDrawData* draw_data);

  static const char* GetClipboardTextBridge();
  const char* GetClipboardText();
  static void SetClipboardTextBridge(const char* text);
  void SetClipboardText(const char* text);

  // GLFW callbacks.
  static void MouseButtonCallbackBridge(GLFWwindow* window,
                                        int button,
                                        int action,
                                        int mods);
  void MouseButtonCallback(int button, int action, int);
  static void ScrollCallbackBridge(GLFWwindow* window,
                                   double xoffset,
                                   double yoffset);
  void ScrollCallback(double, double yoffset);
  static void KeyCallbackBridge(GLFWwindow* window,
                                int key,
                                int scan_code,
                                int action,
                                int mods);
  void KeyCallback(int key, int, int action, int);
  static void CharCallbackBridge(GLFWwindow* window, unsigned int c);
  void CharCallback(unsigned int c);

  GLFWwindow* window_ = nullptr;
  double time_ = 0.0;
  bool mouse_pressed_[3] = {false, false, false};
  float mouse_wheel_ = 0.0f;
  unsigned int font_texture_ = 0;
  Shader shader_;
  int uniform_tex_ = 0;
  int uniform_proj_mtx_ = 0;
  int attrib_position_ = 0;
  int attrib_uv_ = 0;
  int attrib_color_ = 0;
  unsigned int vbo_handle_ = 0;
  unsigned int vao_handle_ = 0;
  unsigned int elements_handle_ = 0;

  // UI component values.
  bool show_main_window_ = true;
  bool show_another_window_ = false;
  ImVec4 clear_color_ = ImColor(114, 144, 154);
  float float_value_ = 0.0f;
};

}  // namespace viewer

#endif  // VIEWER_VIEWER_UI_UI_H_
