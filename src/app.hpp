#pragma once

#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace game::engine {

class Application {
private:
  std::shared_ptr<Window> window_;
  SDL_Event event_;
  VkApplicationInfo vkAppInfo_;
  VkInstance vkInstance_;
  VkDebugUtilsMessengerEXT vkDebugMessenger;

  static Window *_InitWindow(const char *title, VkApplicationInfo &vkAppInfo);
  static VkInstance _initVkInstance();
  static std::optional<VkDebugUtilsMessengerEXT> _GetDebugCreateInfo();

public:
  Application(const char *title);
  ~Application();

  int run();
};
} // namespace game::engine
