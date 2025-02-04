#pragma once

#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <memory>
// #include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace game::engine {

class Application {
private:
  std::shared_ptr<Window> window_;
  SDL_Event event_;
  vk::ApplicationInfo vkAppInfo_;
  vk::Instance vkInstance_;
  vk::DispatchLoaderDynamic vkDldy_;
  vk::DebugUtilsMessengerEXT vkDebugMessenger_;

  static Window *_InitWindow(const char *title, vk::ApplicationInfo &vkAppInfo);
  static VkInstance _initVkInstance(vk::ApplicationInfo &vkAppInfo);
  static std::optional<vk::DebugUtilsMessengerEXT> _initDebugMessenger(vk::Instance instance, vk::DispatchLoaderDynamic dldy);

public:
  Application(const char *title);
  ~Application();

  int run();
};
} // namespace game::engine
