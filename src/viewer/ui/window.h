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

#ifndef VIEWER_UI_WINDOW_H_
#define VIEWER_UI_WINDOW_H_

#include <memory>

#include "viewer/graphics/shader.h"

struct GLFWwindow;
struct ImDrawData;
struct ImFontAtlas;

namespace viewer {

/// @brief A GLFW window.
class Window {
 public:
  /// @brief Construct a new GLFW window.
  /// @param width The inner width of the window.
  /// @param height The inner height of the window.
  /// @param title The title/caption of the window.
  /// @note GLFW must have been initialized before calling the constructor.
  Window(int width, int height, const char* title);

  /// @brief Destroy the GLFW window.
  /// @note Unless the window has already been closed, GLFW must still be
  /// initialzied when destroying the window.
  virtual ~Window();

  /// @brief Check if the window should be closed.
  /// @returns true if the user has requested the window to be closed.
  bool ShouldClose();

  /// @brief Start a new frame for this window.
  ///
  /// This method sets up the OpenGL rendering context for rendering to it. It
  /// also updates the framebuffer dimensions (framebuffer_width and
  /// framebuffer_height).
  void BeginFrame();

  /// @brief End a frame, and swap front & back OpenGL buffers.
  void SwapBuffers();

  int framebuffer_width() const { return framebuffer_width_; }
  int framebuffer_height() const { return framebuffer_height_; }

 protected:
  // Virtual GLFW callback handlers. Override these to catch window events.
  virtual void OnWindowPos(int x, int y);
  virtual void OnWindowSize(int width, int height);
  virtual void OnWindowClose();
  virtual void OnWindowRefresh();
  virtual void OnWindowFocus(bool focused);
  virtual void OnWindowIconify(bool iconified);
  virtual void OnFramebufferSize(int width, int height);
  virtual void OnMouseButton(int button, int action, int mods);
  virtual void OnCursorPos(double x, double y);
  virtual void OnCursorEnter(bool entered);
  virtual void OnScroll(double x_offset, double y_offset);
  virtual void OnKey(int key, int scan_code, int action, int mods);
  virtual void OnChar(unsigned int code_point);
  virtual void OnCharMods(unsigned int code_point, int mods);
  virtual void OnDrop(int count, const char** paths);

  GLFWwindow* glfw_window_ = nullptr;
  int framebuffer_width_ = 0;
  int framebuffer_height_ = 0;

 private:
  // Static bridge functions for GLFW callbacks. These distpatch the call to
  // the designated Window object.
  static void WindowPosDispatch(GLFWwindow* glfw_window, int x, int y);
  static void WindowSizeDispatch(GLFWwindow* glfw_window,
                                 int width,
                                 int height);
  static void WindowCloseDispatch(GLFWwindow* glfw_window);
  static void WindowRefreshDispatch(GLFWwindow* glfw_window);
  static void WindowFocusDispatch(GLFWwindow* glfw_window, int focused);
  static void WindowIconifyDispatch(GLFWwindow* glfw_window, int iconified);
  static void FramebufferSizeDispatch(GLFWwindow* glfw_window,
                                      int width,
                                      int height);
  static void MouseButtonDispatch(GLFWwindow* glfw_window,
                                  int button,
                                  int action,
                                  int mods);
  static void CursorPosDispatch(GLFWwindow* glfw_window, double x, double y);
  static void CursorEnterDispatch(GLFWwindow* glfw_window, int entered);
  static void ScrollDispatch(GLFWwindow* glfw_window,
                             double x_offset,
                             double y_offset);
  static void KeyDispatch(GLFWwindow* glfw_window,
                          int key,
                          int scan_code,
                          int action,
                          int mods);
  static void CharDispatch(GLFWwindow* glfw_window, unsigned int code_point);
  static void CharModsDispatch(GLFWwindow* glfw_window,
                               unsigned int code_point,
                               int mods);
  static void DropDispatch(GLFWwindow* glfw_window,
                           int count,
                           const char** paths);

  // Disable copy/move.
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
};

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
  Shader shader_;
  int uniform_tex_ = 0;
  int uniform_proj_mtx_ = 0;
  int attrib_position_ = 0;
  int attrib_uv_ = 0;
  int attrib_color_ = 0;
  unsigned int vbo_handle_ = 0;
  unsigned int vao_handle_ = 0;
  unsigned int elements_handle_ = 0;
};

}  // namespace viewer

#endif  // VIEWER_UI_WINDOW_H_
