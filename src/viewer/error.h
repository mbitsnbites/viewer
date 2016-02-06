// -*- mode: c++; tab-width: 2; indent-tabs-mode: nil; -*-

#ifndef VIEWER_ERROR_H_
#define VIEWER_ERROR_H_

#include <stdexcept>

namespace viewer {

/// @brief General error.
class Error : public std::runtime_error {
 public:
  Error(const std::string& what_arg) : std::runtime_error(what_arg) {}
  Error(const char* what_arg) : std::runtime_error(what_arg) {}
};

}  // namespace viewer

#endif  // VIEWER_ERROR_H_
