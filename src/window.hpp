#pragma once

#include <SDL3/SDL_video.h>

namespace game::engine {
class Window {
  SDL_Window *window_;

public:
  Window(const char *title, int w, int h);
  ~Window();

  bool IsValid() const;
};
} // namespace game::engine
