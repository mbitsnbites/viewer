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

#include "viewer/ui/window.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "viewer/error.h"

namespace viewer {

Window::Window(int width, int height, const char* title) {
  // Create a window.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfw_window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!glfw_window_) {
    throw Error("Unable to open the GLFW window.");
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
  if (gl3wInit() || !gl3wIsSupported(3, 2)) {
    throw Error("Unable to create an OpenGL 3.2 context.");
  }
}

Window::~Window() {
  if (glfw_window_) {
    glfwDestroyWindow(glfw_window_);
  }
}

bool Window::ShouldClose() {
  return glfwWindowShouldClose(glfw_window_) == GL_TRUE;
}

void Window::Close() {
  if (glfw_window_) {
    glfwDestroyWindow(glfw_window_);
    glfw_window_ = nullptr;
  }
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

Window& Window::GetWindow(GLFWwindow* glfw_window) {
  auto* window =
      reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  if (!window) {
    throw Error("No matching Window found for the given GLFW window handle.");
  }
  return *window;
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
  GetWindow(glfw_window).OnMouseButton(button, action, mods);
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
  GetWindow(glfw_window).OnKey(key, scan_code, action, mods);
}

void Window::CharDispatch(GLFWwindow* glfw_window, unsigned int code_point) {
  GetWindow(glfw_window).OnChar(code_point);
}

void Window::CharModsDispatch(GLFWwindow* glfw_window,
                              unsigned int code_point,
                              int mods) {
  GetWindow(glfw_window).OnCharMods(code_point, mods);
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

void Window::OnMouseButton(int button, int action, int mods) {
  (void)button;
  (void)action;
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

void Window::OnKey(int key, int scan_code, int action, int mods) {
  (void)key;
  (void)scan_code;
  (void)action;
  (void)mods;
}

void Window::OnChar(unsigned int code_point) {
  (void)code_point;
}

void Window::OnCharMods(unsigned int code_point, int mods) {
  (void)code_point;
  (void)mods;
}

void Window::OnDrop(int count, const char** paths) {
  (void)count;
  (void)paths;
}

}  // namespace viewer
