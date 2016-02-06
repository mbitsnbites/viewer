// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-

#include <iostream>

#include "viewer/error.h"
#include "viewer/viewer.h"

int main() {
  try {
    // Start the viewer.
    viewer::Viewer viewer;
    viewer.Run();
  } catch (viewer::Error& e) {
    std::cerr << "Error: " << e.what() << "\n";
  } catch (...) {
    std::cerr << "Error: Unhandled exception.\n";
  }

  return 0;
}
