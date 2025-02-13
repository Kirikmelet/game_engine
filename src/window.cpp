#include "window.hpp"

namespace game::engine {
Window::Window(const char *title, int w, int h) {

  window_ = SDL_CreateWindow(title, w, h, SDL_WINDOW_VULKAN);
}

Window::~Window() { SDL_DestroyWindow(window_); }

bool Window::IsValid() const { return window_ != nullptr; }

SDL_Window* Window::GetRawWindow() { return window_; }

} // namespace game::engine
