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

#include "viewer/ui/ui.h"

#include <iostream>
#include <vector>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#endif  // _WIN32

namespace viewer {

namespace {

// HACK!!!!
Ui* g_ui = nullptr;

}  // namespace

Ui::Ui(GLFWwindow* window) : window_(window) {
  // HACK!!!!
  g_ui = this;
  Init();
}

void Ui::Paint() {
  NewFrame();

  // 1. Show the main window.
  if (show_main_window_) {
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Main Window", &show_main_window_);
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &float_value_, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color_));
    if (ImGui::Button("Another Window"))
      show_another_window_ ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // 2. Show another simple window.
  if (show_another_window_) {
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Another Window", &show_another_window_);
    ImGui::Text("Hello");
    ImGui::End();
  }

  // Rendering.
  glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::Render();
}

void Ui::RenderDrawListsBridge(ImDrawData* draw_data) {
  g_ui->RenderDrawLists(draw_data);
}

// This is the main rendering function that you have to implement and provide to
// ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by
// (0.5f,0.5f) or (0.375f,0.375f)
void Ui::RenderDrawLists(ImDrawData* draw_data) {
  // Backup GL state.
  GLint last_program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_array_buffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer;
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLint last_vertex_array;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
  GLint last_blend_src;
  glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
  GLint last_blend_dst;
  glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb;
  glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha;
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLint last_viewport[4];
  glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  // Setup render state: alpha-blending enabled, no face culling, no depth
  // testing, scissor enabled.
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glActiveTexture(GL_TEXTURE0);

  // Handle cases of screen coordinates != from framebuffer coordinates (e.g.
  // retina displays).
  ImGuiIO& io = ImGui::GetIO();
  int fb_width =
      static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height =
      static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Setup viewport, orthographic projection matrix.
  glViewport(0, 0, static_cast<GLsizei>(fb_width),
             static_cast<GLsizei>(fb_height));
  const float ortho_projection[4][4] = {
      {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
      {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f, 1.0f},
  };
  glUseProgram(shader_handle_);
  glUniform1i(uniform_tex_, 0);
  glUniformMatrix4fv(uniform_proj_mtx_, 1, GL_FALSE, &ortho_projection[0][0]);
  glBindVertexArray(vao_handle_);

  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(cmd_list->VtxBuffer.size()) *
                     static_cast<GLsizeiptr>(sizeof(ImDrawVert)),
                 reinterpret_cast<const GLvoid*>(&cmd_list->VtxBuffer.front()),
                 GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_handle_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(cmd_list->IdxBuffer.size()) *
                     static_cast<GLsizeiptr>(sizeof(ImDrawIdx)),
                 reinterpret_cast<const GLvoid*>(&cmd_list->IdxBuffer.front()),
                 GL_STREAM_DRAW);

    for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin();
         pcmd != cmd_list->CmdBuffer.end(); pcmd++) {
      if (pcmd->UserCallback) {
        pcmd->UserCallback(cmd_list, pcmd);
      } else {
        glBindTexture(
            GL_TEXTURE_2D,
            static_cast<GLuint>(reinterpret_cast<intptr_t>(pcmd->TextureId)));
        glScissor(static_cast<GLint>(pcmd->ClipRect.x),
                  static_cast<GLint>(fb_height - pcmd->ClipRect.w),
                  static_cast<GLint>(pcmd->ClipRect.z - pcmd->ClipRect.x),
                  static_cast<GLint>(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(
            GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
            sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
            idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  glUseProgram(static_cast<GLuint>(last_program));
  glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
  glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(last_array_buffer));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLuint>(last_element_array_buffer));
  glBindVertexArray(static_cast<GLuint>(last_vertex_array));
  glBlendEquationSeparate(static_cast<GLuint>(last_blend_equation_rgb),
                          static_cast<GLuint>(last_blend_equation_alpha));
  glBlendFunc(static_cast<GLuint>(last_blend_src),
              static_cast<GLuint>(last_blend_dst));
  if (last_enable_blend)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
  if (last_enable_cull_face)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
  if (last_enable_depth_test)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test)
    glEnable(GL_SCISSOR_TEST);
  else
    glDisable(GL_SCISSOR_TEST);
  glViewport(last_viewport[0], last_viewport[1],
             static_cast<GLsizei>(last_viewport[2]),
             static_cast<GLsizei>(last_viewport[3]));
}

const char* Ui::GetClipboardTextBridge() {
  return g_ui->GetClipboardText();
}

const char* Ui::GetClipboardText() {
  return glfwGetClipboardString(window_);
}

void Ui::SetClipboardTextBridge(const char* text) {
  g_ui->SetClipboardText(text);
}

void Ui::SetClipboardText(const char* text) {
  glfwSetClipboardString(window_, text);
}

void Ui::MouseButtonCallbackBridge(GLFWwindow*,
                                   int button,
                                   int action,
                                   int mods) {
  g_ui->MouseButtonCallback(button, action, mods);
}

void Ui::MouseButtonCallback(int button, int action, int /* mods */) {
  if (action == GLFW_PRESS && button >= 0 && button < 3)
    mouse_pressed_[button] = true;
}

void Ui::ScrollCallbackBridge(GLFWwindow*, double xoffset, double yoffset) {
  g_ui->ScrollCallback(xoffset, yoffset);
}

void Ui::ScrollCallback(double /* xoffset */, double yoffset) {
  // Use fractional mouse wheel, 1.0 unit 5 lines.
  mouse_wheel_ += static_cast<float>(yoffset);
}

void Ui::KeyCallbackBridge(GLFWwindow*,
                           int key,
                           int scan_code,
                           int action,
                           int mods) {
  g_ui->KeyCallback(key, scan_code, action, mods);
}

void Ui::KeyCallback(int key, int /* scan_code */, int action, int /* mods */) {
  ImGuiIO& io = ImGui::GetIO();
  if (action == GLFW_PRESS)
    io.KeysDown[key] = true;
  if (action == GLFW_RELEASE)
    io.KeysDown[key] = false;

  // Modifiers (mods) are not reliable across systems.
  io.KeyCtrl =
      io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift =
      io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}

void Ui::CharCallbackBridge(GLFWwindow*, unsigned int c) {
  g_ui->CharCallback(c);
}

void Ui::CharCallback(unsigned int c) {
  ImGuiIO& io = ImGui::GetIO();
  if (c > 0 && c < 0x10000)
    io.AddInputCharacter(static_cast<unsigned short>(c));
}

bool Ui::CreateFontsTexture() {
  // Build texture atlas.
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be
  // compatible with user's existing shader.
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  // Upload texture to graphics system.
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &font_texture_);
  glBindTexture(GL_TEXTURE_2D, font_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = reinterpret_cast<void*>(font_texture_);

  // Restore state.
  glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));

  return true;
}

bool Ui::CreateDeviceObjects() {
  // Backup GL state.
  GLint last_texture, last_array_buffer, last_vertex_array;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

  const GLchar* vertex_shader =
      "#version 150\n"
      "uniform mat4 ProjMtx;\n"
      "in vec2 Position;\n"
      "in vec2 UV;\n"
      "in vec4 Color;\n"
      "out vec2 Frag_UV;\n"
      "out vec4 Frag_Color;\n"
      "void main()\n"
      "{\n"
      "	Frag_UV = UV;\n"
      "	Frag_Color = Color;\n"
      "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
      "}\n";

  const GLchar* fragment_shader =
      "#version 150\n"
      "uniform sampler2D Texture;\n"
      "in vec2 Frag_UV;\n"
      "in vec4 Frag_Color;\n"
      "out vec4 Out_Color;\n"
      "void main()\n"
      "{\n"
      "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
      "}\n";

  shader_handle_ = glCreateProgram();
  vert_handle_ = glCreateShader(GL_VERTEX_SHADER);
  frag_handle_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vert_handle_, 1, &vertex_shader, nullptr);
  glShaderSource(frag_handle_, 1, &fragment_shader, nullptr);
  glCompileShader(vert_handle_);
  GLint result;
  glGetShaderiv(vert_handle_, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE) {
    GLsizei len = 0;
    std::vector<GLchar> str(1024);
    glGetShaderInfoLog(vert_handle_, static_cast<GLsizei>(str.size()), &len,
                       str.data());
    // TODO(m): Raise exception instead!
    std::cout << "Vertex shader: " << str.data() << "\n";
  }
  glCompileShader(frag_handle_);
  glGetShaderiv(frag_handle_, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE) {
    GLsizei len = 0;
    std::vector<GLchar> str(1024);
    glGetShaderInfoLog(frag_handle_, static_cast<GLsizei>(str.size()), &len,
                       str.data());
    // TODO(m): Raise exception instead!
    std::cout << "Fragment shader: " << str.data() << "\n";
  }
  glAttachShader(shader_handle_, vert_handle_);
  glAttachShader(shader_handle_, frag_handle_);
  glLinkProgram(shader_handle_);
  glGetProgramiv(shader_handle_, GL_LINK_STATUS, &result);
  if (result != GL_TRUE) {
    GLsizei len = 0;
    std::vector<GLchar> str(1024);
    glGetProgramInfoLog(shader_handle_, static_cast<GLsizei>(str.size()), &len,
                        str.data());
    // TODO(m): Raise exception instead!
    std::cout << "Shader program: " << str.data() << "\n";
  }

  uniform_tex_ = glGetUniformLocation(shader_handle_, "Texture");
  uniform_proj_mtx_ = glGetUniformLocation(shader_handle_, "ProjMtx");
  attrib_position_ = glGetAttribLocation(shader_handle_, "Position");
  attrib_uv_ = glGetAttribLocation(shader_handle_, "UV");
  attrib_color_ = glGetAttribLocation(shader_handle_, "Color");

  glGenBuffers(1, &vbo_handle_);
  glGenBuffers(1, &elements_handle_);

  glGenVertexArrays(1, &vao_handle_);
  glBindVertexArray(vao_handle_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_handle_);
  glEnableVertexAttribArray(static_cast<GLuint>(attrib_position_));
  glEnableVertexAttribArray(static_cast<GLuint>(attrib_uv_));
  glEnableVertexAttribArray(static_cast<GLuint>(attrib_color_));

  glVertexAttribPointer(
      static_cast<GLuint>(attrib_position_), 2, GL_FLOAT, GL_FALSE,
      sizeof(ImDrawVert),
      reinterpret_cast<GLvoid*>(&(reinterpret_cast<ImDrawVert*>(0))->pos));
  glVertexAttribPointer(
      static_cast<GLuint>(attrib_uv_), 2, GL_FLOAT, GL_FALSE,
      sizeof(ImDrawVert),
      reinterpret_cast<GLvoid*>(&(reinterpret_cast<ImDrawVert*>(0))->uv));
  glVertexAttribPointer(
      static_cast<GLuint>(attrib_color_), 4, GL_UNSIGNED_BYTE, GL_TRUE,
      sizeof(ImDrawVert),
      reinterpret_cast<GLvoid*>(&(reinterpret_cast<ImDrawVert*>(0))->col));

  CreateFontsTexture();

  // Restore modified GL state.
  glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
  glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(last_array_buffer));
  glBindVertexArray(static_cast<GLuint>(last_vertex_array));

  return true;
}

void Ui::InvalidateDeviceObjects() {
  if (vao_handle_)
    glDeleteVertexArrays(1, &vao_handle_);
  if (vbo_handle_)
    glDeleteBuffers(1, &vbo_handle_);
  if (elements_handle_)
    glDeleteBuffers(1, &elements_handle_);
  vao_handle_ = vbo_handle_ = elements_handle_ = 0;

  glDetachShader(shader_handle_, vert_handle_);
  glDeleteShader(vert_handle_);
  vert_handle_ = 0;

  glDetachShader(shader_handle_, frag_handle_);
  glDeleteShader(frag_handle_);
  frag_handle_ = 0;

  glDeleteProgram(shader_handle_);
  shader_handle_ = 0;

  if (font_texture_) {
    glDeleteTextures(1, &font_texture_);
    ImGui::GetIO().Fonts->TexID = nullptr;
    font_texture_ = 0;
  }
}

bool Ui::Init() {
  ImGuiIO& io = ImGui::GetIO();
  // Keyboard mapping. ImGui will use those indices to peek into the
  // io.KeyDown[] array.
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after
  // ImGui::Render() to get the same ImDrawData pointer.
  io.RenderDrawListsFn = RenderDrawListsBridge;
  io.SetClipboardTextFn = SetClipboardTextBridge;
  io.GetClipboardTextFn = GetClipboardTextBridge;
#ifdef _WIN32
  io.ImeWindowHandle = glfwGetWin32Window(window_);
#endif

  glfwSetMouseButtonCallback(window_, MouseButtonCallbackBridge);
  glfwSetScrollCallback(window_, ScrollCallbackBridge);
  glfwSetKeyCallback(window_, KeyCallbackBridge);
  glfwSetCharCallback(window_, CharCallbackBridge);

  return true;
}

void Ui::Shutdown() {
  InvalidateDeviceObjects();
  ImGui::Shutdown();
}

void Ui::NewFrame() {
  if (!font_texture_)
    CreateDeviceObjects();

  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(window_, &w, &h);
  glfwGetFramebufferSize(window_, &display_w, &display_h);
  io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
  io.DisplayFramebufferScale = ImVec2(static_cast<float>(display_w) / w,
                                      static_cast<float>(display_h) / h);

  // Setup time step
  double current_time = glfwGetTime();
  io.DeltaTime = time_ > 0.0 ? static_cast<float>(current_time - time_)
                             : static_cast<float>(1.0f / 60.0f);
  time_ = current_time;

  // Setup inputs.
  // (we already got mouse wheel, keyboard keys & characters from GLFW callbacks
  // polled in glfwPollEvents())
  if (glfwGetWindowAttrib(window_, GLFW_FOCUSED)) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(window_, &mouse_x, &mouse_y);
    // Mouse position in screen coordinates (set to -1,-1 if no mouse / on
    // another screen, etc.)
    io.MousePos =
        ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
  } else {
    io.MousePos = ImVec2(-1, -1);
  }

  for (int i = 0; i < 3; i++) {
    // If a mouse press event came, always pass it as "mouse held this frame",
    // so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[i] = mouse_pressed_[i] || glfwGetMouseButton(window_, i) != 0;
    mouse_pressed_[i] = false;
  }

  io.MouseWheel = mouse_wheel_;
  mouse_wheel_ = 0.0f;

  // Hide OS mouse cursor if ImGui is drawing it.
  glfwSetInputMode(window_, GLFW_CURSOR, io.MouseDrawCursor
                                             ? GLFW_CURSOR_HIDDEN
                                             : GLFW_CURSOR_NORMAL);

  // Start the frame.
  ImGui::NewFrame();
}

}  // namespace viewer
