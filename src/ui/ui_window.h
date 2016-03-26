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

#ifndef UI_UI_WINDOW_H_
#define UI_UI_WINDOW_H_

#include <memory>

#include "gfx/shader.h"
#include "ui/window.h"

struct ImDrawData;
struct ImFontAtlas;

namespace ui {

/// @brief A GLFW window with support for ImGui UI rendering.
class UiWindow : public Window {
 public:
  /// @brief Construct a new GLFW window with UI support.
  /// @param width The inner width of the window.
  /// @param height The inner height of the window.
  /// @param title The title/caption of the window.
  /// @note GLFW must have been initialized before calling the constructor.
  UiWindow(int width, int height, const char* title);

  /// @brief Destructor.
  virtual ~UiWindow();

  /// @brief Paint the UI.
  void PaintUi();

 private:
  void CreateDeviceObjects();
  void CreateFontsTexture();

  void BeginUi();
  void EndUi();

  /// @brief Define the UI using ImGui calls.
  /// @note Override this method to render something meaningful.
  virtual void DefineUi();

  static void RenderDrawListsDispatch(ImDrawData* draw_data);
  void RenderDrawLists(ImDrawData* draw_data);

  static const char* GetClipboardText();
  static void SetClipboardText(const char* text);

  static void MouseButtonHandler(GLFWwindow* glfw_window,
                                 int button,
                                 int action,
                                 int mods);
  static void ScrollHandler(GLFWwindow* glfw_window,
                            double x_offset,
                            double y_offset);
  static void KeyHandler(GLFWwindow* glfw_window,
                         int key,
                         int scan_code,
                         int action,
                         int mods);
  static void CharHandler(GLFWwindow* glfw_window, unsigned int code_point);

  void* imgui_context_ = nullptr;
  std::unique_ptr<ImFontAtlas> font_atlas_;

  double time_ = 0.0;
  bool mouse_pressed_[3] = {false, false, false};
  float mouse_wheel_ = 0.0f;
  unsigned int font_texture_ = 0;
  gfx::Shader shader_;
  int uniform_tex_ = 0;
  int uniform_proj_mtx_ = 0;
  int attrib_position_ = 0;
  int attrib_uv_ = 0;
  int attrib_color_ = 0;
  unsigned int vbo_handle_ = 0;
  unsigned int vao_handle_ = 0;
  unsigned int elements_handle_ = 0;
};

}  // namespace ui

#endif  // UI_UI_WINDOW_H_
