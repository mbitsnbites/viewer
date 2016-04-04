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

#include "ui/window.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "base/error.h"

namespace ui {

namespace {

Window& GetWindow(GLFWwindow* glfw_window) {
  auto* window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  if (window == nullptr) {
    throw base::Error(
        "No matching Window found for the given GLFW window handle.");
  }
  return *window;
}

}  // namespace

Modifiers Modifiers::operator+(const Modifiers& other) {
  Modifiers result(*this);
  result += other;
  return result;
}

const Modifiers& Modifiers::operator+=(const Modifiers& other) {
  mask_ |= other.mask_;
  return *this;
}

Window::Window(int width, int height, const char* title) {
  // Create a window.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfw_window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (glfw_window_ == nullptr) {
    throw base::Error("Unable to open the GLFW window.");
  }

  // Get initial window properties.
  glfwGetFramebufferSize(glfw_window_, &framebuffer_width_,
                         &framebuffer_height_);

  // Set up all callback handlers.
  glfwSetWindowUserPointer(glfw_window_, this);
  glfwSetWindowPosCallback(glfw_window_, WindowPosDispatch);
  glfwSetWindowSizeCallback(glfw_window_, WindowSizeDispatch);
  glfwSetWindowCloseCallback(glfw_window_, WindowCloseDispatch);
  glfwSetWindowRefreshCallback(glfw_window_, WindowRefreshDispatch);
  glfwSetWindowFocusCallback(glfw_window_, WindowFocusDispatch);
  glfwSetWindowIconifyCallback(glfw_window_, WindowIconifyDispatch);
  glfwSetFramebufferSizeCallback(glfw_window_, FramebufferSizeDispatch);
  glfwSetKeyCallback(glfw_window_, KeyDispatch);
  glfwSetCharCallback(glfw_window_, CharDispatch);
  glfwSetCharModsCallback(glfw_window_, CharModsDispatch);
  glfwSetMouseButtonCallback(glfw_window_, MouseButtonDispatch);
  glfwSetCursorPosCallback(glfw_window_, CursorPosDispatch);
  glfwSetCursorEnterCallback(glfw_window_, CursorEnterDispatch);
  glfwSetScrollCallback(glfw_window_, ScrollDispatch);
  glfwSetDropCallback(glfw_window_, DropDispatch);

  // Initialize the OpenGL context.
  glfwMakeContextCurrent(glfw_window_);
  if (gl3wInit() != 0 || gl3wIsSupported(3, 2) == 0) {
    throw base::Error("Unable to create an OpenGL 3.2 context.");
  }

  // Enable vertical sync.
  glfwSwapInterval(1);
}

Window::~Window() {
  if (glfw_window_ != nullptr) {
    glfwDestroyWindow(glfw_window_);
  }
}

bool Window::ShouldClose() {
  return glfwWindowShouldClose(glfw_window_) == GL_TRUE;
}

void Window::BeginFrame() {
  // Activate the rendering context.
  glfwMakeContextCurrent(glfw_window_);

  // Update the framebuffer size.
  glfwGetFramebufferSize(glfw_window_, &framebuffer_width_,
                         &framebuffer_height_);
  glViewport(0, 0, framebuffer_width_, framebuffer_height_);
}

void Window::SwapBuffers() {
  glfwSwapBuffers(glfw_window_);
}

MouseButton Window::ToMouseButton(int glfw_mouse_button) {
  switch (glfw_mouse_button) {
    default:
    case GLFW_MOUSE_BUTTON_1:
      return MouseButton::Button1;
    case GLFW_MOUSE_BUTTON_2:
      return MouseButton::Button2;
    case GLFW_MOUSE_BUTTON_3:
      return MouseButton::Button3;
    case GLFW_MOUSE_BUTTON_4:
      return MouseButton::Button4;
    case GLFW_MOUSE_BUTTON_5:
      return MouseButton::Button5;
    case GLFW_MOUSE_BUTTON_6:
      return MouseButton::Button6;
    case GLFW_MOUSE_BUTTON_7:
      return MouseButton::Button7;
    case GLFW_MOUSE_BUTTON_8:
      return MouseButton::Button8;
  }
}

KeyCode Window::ToKeyCode(int glfw_key_code) {
  switch (glfw_key_code) {
    default:
    case GLFW_KEY_UNKNOWN:
      return KeyCode::Unknown;
    case GLFW_KEY_SPACE:
      return KeyCode::Space;
    case GLFW_KEY_APOSTROPHE:
      return KeyCode::Apostrophe;
    case GLFW_KEY_COMMA:
      return KeyCode::Comma;
    case GLFW_KEY_MINUS:
      return KeyCode::Minus;
    case GLFW_KEY_PERIOD:
      return KeyCode::Period;
    case GLFW_KEY_SLASH:
      return KeyCode::Slash;
    case GLFW_KEY_0:
      return KeyCode::Zero;
    case GLFW_KEY_1:
      return KeyCode::One;
    case GLFW_KEY_2:
      return KeyCode::Two;
    case GLFW_KEY_3:
      return KeyCode::Three;
    case GLFW_KEY_4:
      return KeyCode::Four;
    case GLFW_KEY_5:
      return KeyCode::Five;
    case GLFW_KEY_6:
      return KeyCode::Six;
    case GLFW_KEY_7:
      return KeyCode::Seven;
    case GLFW_KEY_8:
      return KeyCode::Eight;
    case GLFW_KEY_9:
      return KeyCode::Nine;
    case GLFW_KEY_SEMICOLON:
      return KeyCode::Semicolon;
    case GLFW_KEY_EQUAL:
      return KeyCode::Equal;
    case GLFW_KEY_A:
      return KeyCode::A;
    case GLFW_KEY_B:
      return KeyCode::B;
    case GLFW_KEY_C:
      return KeyCode::C;
    case GLFW_KEY_D:
      return KeyCode::D;
    case GLFW_KEY_E:
      return KeyCode::E;
    case GLFW_KEY_F:
      return KeyCode::F;
    case GLFW_KEY_G:
      return KeyCode::G;
    case GLFW_KEY_H:
      return KeyCode::H;
    case GLFW_KEY_I:
      return KeyCode::I;
    case GLFW_KEY_J:
      return KeyCode::J;
    case GLFW_KEY_K:
      return KeyCode::K;
    case GLFW_KEY_L:
      return KeyCode::L;
    case GLFW_KEY_M:
      return KeyCode::M;
    case GLFW_KEY_N:
      return KeyCode::N;
    case GLFW_KEY_O:
      return KeyCode::O;
    case GLFW_KEY_P:
      return KeyCode::P;
    case GLFW_KEY_Q:
      return KeyCode::Q;
    case GLFW_KEY_R:
      return KeyCode::R;
    case GLFW_KEY_S:
      return KeyCode::S;
    case GLFW_KEY_T:
      return KeyCode::T;
    case GLFW_KEY_U:
      return KeyCode::U;
    case GLFW_KEY_V:
      return KeyCode::V;
    case GLFW_KEY_W:
      return KeyCode::W;
    case GLFW_KEY_X:
      return KeyCode::X;
    case GLFW_KEY_Y:
      return KeyCode::Y;
    case GLFW_KEY_Z:
      return KeyCode::Z;
    case GLFW_KEY_LEFT_BRACKET:
      return KeyCode::LeftBracket;
    case GLFW_KEY_BACKSLASH:
      return KeyCode::Backslash;
    case GLFW_KEY_RIGHT_BRACKET:
      return KeyCode::RightBracket;
    case GLFW_KEY_GRAVE_ACCENT:
      return KeyCode::GraveAccent;
    case GLFW_KEY_WORLD_1:
      return KeyCode::World1;
    case GLFW_KEY_WORLD_2:
      return KeyCode::World2;
    case GLFW_KEY_ESCAPE:
      return KeyCode::Escape;
    case GLFW_KEY_ENTER:
      return KeyCode::Enter;
    case GLFW_KEY_TAB:
      return KeyCode::Tab;
    case GLFW_KEY_BACKSPACE:
      return KeyCode::Backspace;
    case GLFW_KEY_INSERT:
      return KeyCode::Insert;
    case GLFW_KEY_DELETE:
      return KeyCode::Delete;
    case GLFW_KEY_RIGHT:
      return KeyCode::Right;
    case GLFW_KEY_LEFT:
      return KeyCode::Left;
    case GLFW_KEY_DOWN:
      return KeyCode::Down;
    case GLFW_KEY_UP:
      return KeyCode::Up;
    case GLFW_KEY_PAGE_UP:
      return KeyCode::PageUp;
    case GLFW_KEY_PAGE_DOWN:
      return KeyCode::PageDown;
    case GLFW_KEY_HOME:
      return KeyCode::Home;
    case GLFW_KEY_END:
      return KeyCode::End;
    case GLFW_KEY_CAPS_LOCK:
      return KeyCode::CapsLock;
    case GLFW_KEY_SCROLL_LOCK:
      return KeyCode::ScrollLock;
    case GLFW_KEY_NUM_LOCK:
      return KeyCode::NumLock;
    case GLFW_KEY_PRINT_SCREEN:
      return KeyCode::PrintScreen;
    case GLFW_KEY_PAUSE:
      return KeyCode::Pause;
    case GLFW_KEY_F1:
      return KeyCode::F1;
    case GLFW_KEY_F2:
      return KeyCode::F2;
    case GLFW_KEY_F3:
      return KeyCode::F3;
    case GLFW_KEY_F4:
      return KeyCode::F4;
    case GLFW_KEY_F5:
      return KeyCode::F5;
    case GLFW_KEY_F6:
      return KeyCode::F6;
    case GLFW_KEY_F7:
      return KeyCode::F7;
    case GLFW_KEY_F8:
      return KeyCode::F8;
    case GLFW_KEY_F9:
      return KeyCode::F9;
    case GLFW_KEY_F10:
      return KeyCode::F10;
    case GLFW_KEY_F11:
      return KeyCode::F11;
    case GLFW_KEY_F12:
      return KeyCode::F12;
    case GLFW_KEY_F13:
      return KeyCode::F13;
    case GLFW_KEY_F14:
      return KeyCode::F14;
    case GLFW_KEY_F15:
      return KeyCode::F15;
    case GLFW_KEY_F16:
      return KeyCode::F16;
    case GLFW_KEY_F17:
      return KeyCode::F17;
    case GLFW_KEY_F18:
      return KeyCode::F18;
    case GLFW_KEY_F19:
      return KeyCode::F19;
    case GLFW_KEY_F20:
      return KeyCode::F20;
    case GLFW_KEY_F21:
      return KeyCode::F21;
    case GLFW_KEY_F22:
      return KeyCode::F22;
    case GLFW_KEY_F23:
      return KeyCode::F23;
    case GLFW_KEY_F24:
      return KeyCode::F24;
    case GLFW_KEY_F25:
      return KeyCode::F25;
    case GLFW_KEY_KP_0:
      return KeyCode::Kp0;
    case GLFW_KEY_KP_1:
      return KeyCode::Kp1;
    case GLFW_KEY_KP_2:
      return KeyCode::Kp2;
    case GLFW_KEY_KP_3:
      return KeyCode::Kp3;
    case GLFW_KEY_KP_4:
      return KeyCode::Kp4;
    case GLFW_KEY_KP_5:
      return KeyCode::Kp5;
    case GLFW_KEY_KP_6:
      return KeyCode::Kp6;
    case GLFW_KEY_KP_7:
      return KeyCode::Kp7;
    case GLFW_KEY_KP_8:
      return KeyCode::Kp8;
    case GLFW_KEY_KP_9:
      return KeyCode::Kp9;
    case GLFW_KEY_KP_DECIMAL:
      return KeyCode::KpDecimal;
    case GLFW_KEY_KP_DIVIDE:
      return KeyCode::KpDivide;
    case GLFW_KEY_KP_MULTIPLY:
      return KeyCode::KpMultiply;
    case GLFW_KEY_KP_SUBTRACT:
      return KeyCode::KpSubtract;
    case GLFW_KEY_KP_ADD:
      return KeyCode::KpAdd;
    case GLFW_KEY_KP_ENTER:
      return KeyCode::KpEnter;
    case GLFW_KEY_KP_EQUAL:
      return KeyCode::KpEqual;
    case GLFW_KEY_LEFT_SHIFT:
      return KeyCode::LeftShift;
    case GLFW_KEY_LEFT_CONTROL:
      return KeyCode::LeftControl;
    case GLFW_KEY_LEFT_ALT:
      return KeyCode::LeftAlt;
    case GLFW_KEY_LEFT_SUPER:
      return KeyCode::LeftSuper;
    case GLFW_KEY_RIGHT_SHIFT:
      return KeyCode::RightShift;
    case GLFW_KEY_RIGHT_CONTROL:
      return KeyCode::RightControl;
    case GLFW_KEY_RIGHT_ALT:
      return KeyCode::RightAlt;
    case GLFW_KEY_RIGHT_SUPER:
      return KeyCode::RightSuper;
    case GLFW_KEY_MENU:
      return KeyCode::Menu;
  }
}

Modifiers Window::ToModifiers(int glfw_modifiers) {
  Modifiers result;
  if ((glfw_modifiers & GLFW_MOD_SHIFT) != 0) {
    result += Modifiers::Shift();
  }
  if ((glfw_modifiers & GLFW_MOD_CONTROL) != 0) {
    result += Modifiers::Control();
  }
  if ((glfw_modifiers & GLFW_MOD_ALT) != 0) {
    result += Modifiers::Alt();
  }
  if ((glfw_modifiers & GLFW_MOD_SUPER) != 0) {
    result += Modifiers::Super();
  }
  return result;
}

//------------------------------------------------------------------------------
// GLFW callback dispatch functions.

void Window::WindowPosDispatch(GLFWwindow* glfw_window, int x, int y) {
  GetWindow(glfw_window).OnWindowPos(x, y);
}

void Window::WindowSizeDispatch(GLFWwindow* glfw_window,
                                int width,
                                int height) {
  GetWindow(glfw_window).OnWindowSize(width, height);
}

void Window::WindowCloseDispatch(GLFWwindow* glfw_window) {
  GetWindow(glfw_window).OnWindowClose();
}

void Window::WindowRefreshDispatch(GLFWwindow* glfw_window) {
  GetWindow(glfw_window).OnWindowRefresh();
}

void Window::WindowFocusDispatch(GLFWwindow* glfw_window, int focused) {
  GetWindow(glfw_window).OnWindowFocus(focused == GL_TRUE);
}

void Window::WindowIconifyDispatch(GLFWwindow* glfw_window, int iconified) {
  GetWindow(glfw_window).OnWindowIconify(iconified == GL_TRUE);
}

void Window::FramebufferSizeDispatch(GLFWwindow* glfw_window,
                                     int width,
                                     int height) {
  GetWindow(glfw_window).OnFramebufferSize(width, height);
}

void Window::MouseButtonDispatch(GLFWwindow* glfw_window,
                                 int button,
                                 int action,
                                 int mods) {
  GetWindow(glfw_window)
      .OnMouseButton(ToMouseButton(button), action == GLFW_PRESS,
                     ToModifiers(mods));
}

void Window::CursorPosDispatch(GLFWwindow* glfw_window, double x, double y) {
  GetWindow(glfw_window).OnCursorPos(x, y);
}

void Window::CursorEnterDispatch(GLFWwindow* glfw_window, int entered) {
  GetWindow(glfw_window).OnCursorEnter(entered == GL_TRUE);
}

void Window::ScrollDispatch(GLFWwindow* glfw_window,
                            double x_offset,
                            double y_offset) {
  GetWindow(glfw_window).OnScroll(x_offset, y_offset);
}

void Window::KeyDispatch(GLFWwindow* glfw_window,
                         int key,
                         int scan_code,
                         int action,
                         int mods) {
  GetWindow(glfw_window)
      .OnKey(ToKeyCode(key), scan_code, action == GLFW_PRESS,
             ToModifiers(mods));
}

void Window::CharDispatch(GLFWwindow* glfw_window, unsigned int code_point) {
  GetWindow(glfw_window).OnChar(code_point);
}

void Window::CharModsDispatch(GLFWwindow* glfw_window,
                              unsigned int code_point,
                              int mods) {
  GetWindow(glfw_window).OnCharMods(code_point, ToModifiers(mods));
}

void Window::DropDispatch(GLFWwindow* glfw_window,
                          int count,
                          const char** paths) {
  GetWindow(glfw_window).OnDrop(count, paths);
}

//------------------------------------------------------------------------------
// Default event handlers do nothing.

void Window::OnWindowPos(int x, int y) {
  (void)x;
  (void)y;
}

void Window::OnWindowSize(int width, int height) {
  (void)width;
  (void)height;
}

void Window::OnWindowClose() {
}

void Window::OnWindowRefresh() {
}

void Window::OnWindowFocus(bool focused) {
  (void)focused;
}

void Window::OnWindowIconify(bool iconified) {
  (void)iconified;
}

void Window::OnFramebufferSize(int width, int height) {
  (void)width;
  (void)height;
}

void Window::OnMouseButton(MouseButton button, bool pressed, Modifiers mods) {
  (void)button;
  (void)pressed;
  (void)mods;
}

void Window::OnCursorPos(double x, double y) {
  (void)x;
  (void)y;
}

void Window::OnCursorEnter(bool entered) {
  (void)entered;
}

void Window::OnScroll(double x_offset, double y_offset) {
  (void)x_offset;
  (void)y_offset;
}

void Window::OnKey(KeyCode key, int scan_code, bool pressed, Modifiers mods) {
  (void)key;
  (void)scan_code;
  (void)pressed;
  (void)mods;
}

void Window::OnChar(unsigned int code_point) {
  (void)code_point;
}

void Window::OnCharMods(unsigned int code_point, Modifiers mods) {
  (void)code_point;
  (void)mods;
}

void Window::OnDrop(int count, const char** paths) {
  (void)count;
  (void)paths;
}

}  // namespace ui
