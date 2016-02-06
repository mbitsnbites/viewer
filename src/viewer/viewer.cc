// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-

#include <iostream>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

namespace {

// Startup window properties.
const int kWinWidth = 1280;
const int kWinHeight = 720;
const char* kWinTitle = "Viewer";

}  // namespace

int main() {
  if (!glfwInit()) {
    return -1;
  }

  // Create a window.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  auto* window =
      glfwCreateWindow(kWinWidth, kWinHeight, kWinTitle, nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Initialize the OpenGL context.
  glfwMakeContextCurrent(window);
  if (gl3wInit() || !gl3wIsSupported(3, 2)) {
    std::cerr << "Unable to create an OpenGL 3.2 context.\n";
    glfwTerminate();
    return -1;
  }

  // Main loop.
  while (!glfwWindowShouldClose(window)) {
    glClearColor(1.0, 0.6, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
