#include "Window.hpp"

namespace window {
Window::Window(std::string title, uint32_t width, uint32_t height) {
  this->title = title;
  this->width = width;
  this->height = height;

  this->window =
      glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

Window::~Window() { glfwDestroyWindow(this->window); }

void Window::set_current_context() { glfwMakeContextCurrent(this->window); }

void Window::set_background_color(float bg_color[4]) {
  this->background_color[0] = bg_color[0];
  this->background_color[1] = bg_color[1];
  this->background_color[2] = bg_color[2];
  this->background_color[3] = bg_color[3];
}

void Window::update() {
  this->set_current_context();

  glfwSwapBuffers(this->window);

  // OpenGL (será substituído por Vulkan)
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(this->background_color[0], this->background_color[1],
               this->background_color[2], this->background_color[3]);

  // Carregar todos os objetos, ...

  glfwPollEvents();
}

bool Window::should_close() { return glfwWindowShouldClose(this->window); }
} // namespace window
