#pragma once

#include "devices.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <memory>
// #include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace game::engine {

class Application {
private:
  std::string title_ = std::string("Hello World!");
  std::shared_ptr<Window> window_;
  vk::ApplicationInfo vkAppInfo_;
  SDL_Event event_;
  renderer::RendererBase renderer_;

public:
  Application(const char *title);
  ~Application();

  int run();
};
} // namespace game::engine
