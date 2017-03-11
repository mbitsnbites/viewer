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

#ifndef UI_WINDOW_H_
#define UI_WINDOW_H_

#include <memory>

struct GLFWwindow;

namespace ui {

/// @brief Mouse button identifiers.
enum class MouseButton {
  Button1,
  Button2,
  Button3,
  Button4,
  Button5,
  Button6,
  Button7,
  Button8
};

/// @brief Keyboard key codes.
enum class KeyCode {
  Unknown,
  Space,
  Apostrophe,
  Comma,
  Minus,
  Period,
  Slash,
  Zero,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Semicolon,
  Equal,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  LeftBracket,
  Backslash,
  RightBracket,
  GraveAccent,
  World1,
  World2,
  Escape,
  Enter,
  Tab,
  Backspace,
  Insert,
  Delete,
  Right,
  Left,
  Down,
  Up,
  PageUp,
  PageDown,
  Home,
  End,
  CapsLock,
  ScrollLock,
  NumLock,
  PrintScreen,
  Pause,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  F25,
  Kp0,
  Kp1,
  Kp2,
  Kp3,
  Kp4,
  Kp5,
  Kp6,
  Kp7,
  Kp8,
  Kp9,
  KpDecimal,
  KpDivide,
  KpMultiply,
  KpSubtract,
  KpAdd,
  KpEnter,
  KpEqual,
  LeftShift,
  LeftControl,
  LeftAlt,
  LeftSuper,
  RightShift,
  RightControl,
  RightAlt,
  RightSuper,
  Menu
};

/// @brief Keyboard modifiers.
class Modifiers {
 public:
  Modifiers() {}
  Modifiers(const Modifiers& other) = default;
  Modifiers& operator=(const Modifiers& other) = default;

  bool operator==(const Modifiers& other) { return other.mask_ == mask_; }
  Modifiers operator+(const Modifiers& other);
  const Modifiers& operator+=(const Modifiers& other);

  static Modifiers Shift() { return Modifiers(Flag::Shift); }
  static Modifiers Control() { return Modifiers(Flag::Control); }
  static Modifiers Alt() { return Modifiers(Flag::Alt); }
  static Modifiers Super() { return Modifiers(Flag::Super); }

  bool IsEmpty() const { return mask_ == 0; }
  bool HasShift() const { return HasFlag(Flag::Shift); }
  bool HasControl() const { return HasFlag(Flag::Control); }
  bool HasAlt() const { return HasFlag(Flag::Alt); }
  bool HasSuper() const { return HasFlag(Flag::Super); }

 private:
  enum class Flag { Shift = 1, Control = 2, Alt = 4, Super = 8 };

  explicit Modifiers(Flag flag) : mask_(static_cast<int>(flag)) {}

  bool HasFlag(Flag flag) const {
    return (mask_ & static_cast<int>(flag)) != 0;
  }

  int mask_ = 0;
};

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
  GLFWwindow* glfw_window() const { return glfw_window_; }

 protected:
  // Convert a GLFW mouse button identifier to a MouseButton.
  static MouseButton ToMouseButton(int glfw_mouse_button);

  // Convert a GLFW key code to a KeyCode.
  static KeyCode ToKeyCode(int glfw_key_code);

  // Convert a GLFW modifier mask to a Modifiers.
  static Modifiers ToModifiers(int glfw_modifiers);

  // Virtual GLFW callback handlers. Override these to catch window events.
  virtual void OnWindowPos(int x, int y);
  virtual void OnWindowSize(int width, int height);
  virtual void OnWindowClose();
  virtual void OnWindowRefresh();
  virtual void OnWindowFocus(bool focused);
  virtual void OnWindowIconify(bool iconified);
  virtual void OnFramebufferSize(int width, int height);
  virtual void OnMouseButton(MouseButton button, bool pressed, Modifiers mods);
  virtual void OnCursorPos(double x, double y);
  virtual void OnCursorEnter(bool entered);
  virtual void OnScroll(double x_offset, double y_offset);
  virtual void OnKey(KeyCode key, int scan_code, bool pressed, Modifiers mods);
  virtual void OnChar(unsigned int code_point);
  virtual void OnCharMods(unsigned int code_point, Modifiers mods);
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

}  // namespace ui

#endif  // UI_WINDOW_H_
