// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-

#include "viewer/viewer.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "viewer/error.h"

namespace viewer {

namespace {

// Startup window properties.
const int kWinWidth = 1280;
const int kWinHeight = 720;
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

  // Main loop.
  while (!glfwWindowShouldClose(window_)) {
    glClearColor(1.0, 0.6, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

}  // namespace viewer
