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

#ifndef GFX_SHADER_H_
#define GFX_SHADER_H_

namespace gfx {

/// @brief An OpenGL shader program.
class Shader {
 public:
  void Compile(const char* vert_src, const char* frag_src);
  void Delete();

  int GetAttribLocation(const char* name);
  int GetUniformLocation(const char* name);
  void UseProgram();

  unsigned int handle() const { return handle_; }
  bool linked() const { return linked_; }

 private:
  unsigned int handle_ = 0;
  unsigned int vert_handle_ = 0;
  unsigned int frag_handle_ = 0;
  bool linked_ = false;
};

}  // namespace gfx

#endif  // GFX_SHADER_H_
