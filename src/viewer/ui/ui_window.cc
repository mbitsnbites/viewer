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

#include "viewer/ui/ui_window.h"

#include <cstdlib>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#endif  // _WIN32

#include "viewer/error.h"
#include "viewer/utils/make_unique.h"

namespace viewer {

namespace {

UiWindow* g_painting_ui_window = nullptr;

UiWindow& GetUiWindow(GLFWwindow* glfw_window) {
  auto* window =
      reinterpret_cast<UiWindow*>(glfwGetWindowUserPointer(glfw_window));
  if (window == nullptr) {
    throw Error("No matching Window found for the given GLFW window handle.");
  }
  return *window;
}

}  // namespace

UiWindow::UiWindow(int width, int height, const char* title)
    : Window(width, height, title) {
  // Override some of the GLFW callbacks, since we need to intercept the user
  // input and pass it on to ImGui.
  glfwSetMouseButtonCallback(glfw_window_, MouseButtonHandler);
  glfwSetScrollCallback(glfw_window_, ScrollHandler);
  glfwSetKeyCallback(glfw_window_, KeyHandler);
  glfwSetCharCallback(glfw_window_, CharHandler);

  // Create a new ImGui context.
  imgui_context_ = std::malloc(ImGui::GetInternalStateSize());
  if (imgui_context_ == nullptr) {
    throw Error("Could not create ImGui context.");
  }
  ImGui::SetInternalState(imgui_context_, true);

  ImGuiIO& io = ImGui::GetIO();

  // Disable settings & logging.
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;

  // Create a new font atlas for this context.
  font_atlas_ = make_unique<ImFontAtlas>();
  io.Fonts = font_atlas_.get();

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
  io.RenderDrawListsFn = RenderDrawListsDispatch;
  io.SetClipboardTextFn = SetClipboardText;
  io.GetClipboardTextFn = GetClipboardText;
#ifdef _WIN32
  io.ImeWindowHandle = glfwGetWin32Window(glfw_window_);
#endif

  CreateDeviceObjects();
}

UiWindow::~UiWindow() {
  ImGui::SetInternalState(imgui_context_);

  if (vao_handle_) {
    glDeleteVertexArrays(1, &vao_handle_);
  }
  if (vbo_handle_) {
    glDeleteBuffers(1, &vbo_handle_);
  }
  if (elements_handle_) {
    glDeleteBuffers(1, &elements_handle_);
  }
  vao_handle_ = vbo_handle_ = elements_handle_ = 0;

  shader_.Delete();

  if (font_texture_) {
    glDeleteTextures(1, &font_texture_);
    ImGui::GetIO().Fonts->TexID = nullptr;
    font_texture_ = 0;
  }

  // Free the ImGui context resources.
  ImGui::Shutdown();
  std::free(imgui_context_);
  ImGui::SetInternalState(nullptr);
}

void UiWindow::PaintUi() {
  BeginUi();
  DefineUi();
  EndUi();
}

void UiWindow::BeginUi() {
  // Activate this window for UI rendering.
  if (g_painting_ui_window != nullptr) {
    throw Error("There is already an active UI window for this thread.");
  }
  g_painting_ui_window = this;
  ImGui::SetInternalState(imgui_context_);

  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(glfw_window_, &w, &h);
  glfwGetFramebufferSize(glfw_window_, &display_w, &display_h);
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
  if (glfwGetWindowAttrib(glfw_window_, GLFW_FOCUSED) == GL_TRUE) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(glfw_window_, &mouse_x, &mouse_y);
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
    io.MouseDown[i] =
        mouse_pressed_[i] || glfwGetMouseButton(glfw_window_, i) != 0;
    mouse_pressed_[i] = false;
  }

  io.MouseWheel = mouse_wheel_;
  mouse_wheel_ = 0.0f;

  // Hide OS mouse cursor if ImGui is drawing it.
  glfwSetInputMode(glfw_window_, GLFW_CURSOR, io.MouseDrawCursor
                                                  ? GLFW_CURSOR_HIDDEN
                                                  : GLFW_CURSOR_NORMAL);

  // Start the frame.
  ImGui::NewFrame();
}

void UiWindow::EndUi() {
  if (g_painting_ui_window == nullptr) {
    throw Error("No active UI window.");
  }
  ImGui::Render();
  g_painting_ui_window = nullptr;
  ImGui::SetInternalState(nullptr);
}

void UiWindow::DefineUi() {
  ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
  ImGui::Begin("A Window");
  ImGui::Text("Override DefineUi() to define your own UI.");
  ImGui::End();
}

void UiWindow::CreateDeviceObjects() {
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
      "  Frag_UV = UV;\n"
      "  Frag_Color = Color;\n"
      "  gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
      "}\n";

  const GLchar* fragment_shader =
      "#version 150\n"
      "uniform sampler2D Texture;\n"
      "in vec2 Frag_UV;\n"
      "in vec4 Frag_Color;\n"
      "out vec4 Out_Color;\n"
      "void main()\n"
      "{\n"
      "  Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
      "}\n";

  shader_.Compile(vertex_shader, fragment_shader);

  uniform_tex_ = shader_.GetUniformLocation("Texture");
  uniform_proj_mtx_ = shader_.GetUniformLocation("ProjMtx");
  attrib_position_ = shader_.GetAttribLocation("Position");
  attrib_uv_ = shader_.GetAttribLocation("UV");
  attrib_color_ = shader_.GetAttribLocation("Color");

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
}

void UiWindow::CreateFontsTexture() {
  // Build texture atlas.
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
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

  // Store our texture ID for later reference (passed to RenderDrawLists).
  io.Fonts->TexID = reinterpret_cast<void*>(font_texture_);

  // Restore OpenGL state.
  glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
}

void UiWindow::RenderDrawListsDispatch(ImDrawData* draw_data) {
  if (g_painting_ui_window == nullptr) {
    throw Error("No active UI window.");
  }
  g_painting_ui_window->RenderDrawLists(draw_data);
}

void UiWindow::RenderDrawLists(ImDrawData* draw_data) {
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
  shader_.UseProgram();
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
      if (pcmd->UserCallback != nullptr) {
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

  // Restore modified GL state.
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
  if (last_enable_blend == GL_TRUE) {
    glEnable(GL_BLEND);
  } else {
    glDisable(GL_BLEND);
  }
  if (last_enable_cull_face == GL_TRUE) {
    glEnable(GL_CULL_FACE);
  } else {
    glDisable(GL_CULL_FACE);
  }
  if (last_enable_depth_test == GL_TRUE) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  if (last_enable_scissor_test == GL_TRUE) {
    glEnable(GL_SCISSOR_TEST);
  } else {
    glDisable(GL_SCISSOR_TEST);
  }
  glViewport(last_viewport[0], last_viewport[1],
             static_cast<GLsizei>(last_viewport[2]),
             static_cast<GLsizei>(last_viewport[3]));
}

const char* UiWindow::GetClipboardText() {
  if (g_painting_ui_window == nullptr) {
    throw Error("No active UI window.");
  }
  return glfwGetClipboardString(g_painting_ui_window->glfw_window_);
}

void UiWindow::SetClipboardText(const char* text) {
  if (g_painting_ui_window == nullptr) {
    throw Error("No active UI window.");
  }
  glfwSetClipboardString(g_painting_ui_window->glfw_window_, text);
}

void UiWindow::MouseButtonHandler(GLFWwindow* glfw_window,
                                  int button,
                                  int action,
                                  int mods) {
  auto& window = GetUiWindow(glfw_window);
  if (action == GLFW_PRESS && button >= 0 && button < 3) {
    window.mouse_pressed_[button] = true;
  }

  window.OnMouseButton(button, action, mods);
}

void UiWindow::ScrollHandler(GLFWwindow* glfw_window,
                             double x_offset,
                             double y_offset) {
  auto& window = GetUiWindow(glfw_window);
  window.mouse_wheel_ += static_cast<float>(y_offset);

  window.OnScroll(x_offset, y_offset);
}

void UiWindow::KeyHandler(GLFWwindow* glfw_window,
                          int key,
                          int scan_code,
                          int action,
                          int mods) {
  auto& window = GetUiWindow(glfw_window);

  ImGui::SetInternalState(window.imgui_context_);
  ImGuiIO& io = ImGui::GetIO();
  if (action == GLFW_PRESS) {
    io.KeysDown[key] = true;
  }
  if (action == GLFW_RELEASE) {
    io.KeysDown[key] = false;
  }

  // Modifiers (mods) are not reliable across systems.
  io.KeyCtrl =
      io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift =
      io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];

  window.OnKey(key, scan_code, action, mods);
}

void UiWindow::CharHandler(GLFWwindow* glfw_window, unsigned int code_point) {
  auto& window = GetUiWindow(glfw_window);
  ImGui::SetInternalState(window.imgui_context_);
  ImGuiIO& io = ImGui::GetIO();
  if (code_point > 0 && code_point < 0x10000) {
    io.AddInputCharacter(static_cast<ImWchar>(code_point));
  }

  window.OnChar(code_point);
}

}  // namespace viewer
