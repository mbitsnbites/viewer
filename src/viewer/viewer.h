// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-

#ifndef VIEWER_VIEWER_H_
#define VIEWER_VIEWER_H_

struct GLFWwindow;

namespace viewer {

/// @brief The viewer application instance.
class Viewer {
 public:
  Viewer();
  ~Viewer();

  void Run();

 private:
  void CreateWindow();

  GLFWwindow* window_ = nullptr;
};

}  // namespace viewer

#endif  // VIEWER_ERROR_H_
