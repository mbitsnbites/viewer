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

#include "viewer/graphics/shader.h"

#include <iostream>
#include <vector>

#include "GL/gl3w.h"

namespace viewer {

namespace {

bool CheckShaderStatus(GLuint handle) {
  GLint result = GL_FALSE;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE) {
    GLsizei len = 0;
    std::vector<GLchar> str(1024);
    glGetShaderInfoLog(handle, static_cast<GLsizei>(str.size()), &len,
                       str.data());
    // TODO(m): Raise exception instead?
    std::cerr << "Shader error: " << str.data() << "\n";
  }
  return result == GL_TRUE;
}

bool CheckProgramStatus(GLuint handle) {
  GLint result;
  glGetProgramiv(handle, GL_LINK_STATUS, &result);
  if (result != GL_TRUE) {
    GLsizei len = 0;
    std::vector<GLchar> str(1024);
    glGetProgramInfoLog(handle, static_cast<GLsizei>(str.size()), &len,
                        str.data());
    // TODO(m): Raise exception instead?
    std::cerr << "Shader program: " << str.data() << "\n";
  }
  return result == GL_TRUE;
}

}  // namespace

void Shader::Compile(const char* vert_src, const char* frag_src) {
  linked_ = false;

  handle_ = glCreateProgram();
  vert_handle_ = glCreateShader(GL_VERTEX_SHADER);
  frag_handle_ = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vert_handle_, 1, &vert_src, nullptr);
  glCompileShader(vert_handle_);
  if (!CheckShaderStatus(vert_handle_)) {
    return;
  }

  glShaderSource(frag_handle_, 1, &frag_src, nullptr);
  glCompileShader(frag_handle_);
  if (!CheckShaderStatus(frag_handle_)) {
    return;
  }

  glAttachShader(handle_, vert_handle_);
  glAttachShader(handle_, frag_handle_);
  glLinkProgram(handle_);
  if (!CheckProgramStatus(handle_)) {
    return;
  }

  linked_ = true;
}

void Shader::Delete() {
  if (handle_) {
    if (vert_handle_) {
      glDetachShader(handle_, vert_handle_);
      glDeleteShader(vert_handle_);
      vert_handle_ = 0;
    }

    if (frag_handle_) {
      glDetachShader(handle_, frag_handle_);
      glDeleteShader(frag_handle_);
      frag_handle_ = 0;
    }

    glDeleteProgram(handle_);
    handle_ = 0;
  }

  linked_ = false;
}

int Shader::GetAttribLocation(const char* name) {
  return linked_ ? glGetAttribLocation(handle_, name) : -1;
}

int Shader::GetUniformLocation(const char* name) {
  return linked_ ? glGetUniformLocation(handle_, name) : -1;
}

void Shader::UseProgram() {
  if (linked_) {
    glUseProgram(handle_);
  }
}

}  // namespace viewer
