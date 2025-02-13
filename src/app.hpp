#pragma once

#include "devices.hpp"
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
  SDL_Event event_;
  vk::ApplicationInfo vkAppInfo_;
  vk::Instance vkInstance_;
  vk::DispatchLoaderDynamic vkDldy_;
  vk::DebugUtilsMessengerEXT vkDebugMessenger_;
  vk::PhysicalDevice vkPhysicalDevice_;
  vk::Device vkDevice_;
  QueueFamilyIndicies vkQueueFamily_;
  std::vector<const char*> vkEnabledLayers_;
  std::vector<const char*> vkExtensionNames_;
  vk::SurfaceKHR vkSurface_;
  std::vector<vk::Queue> vkQueues_;

  void _InitWindow();
  void _InitVkInstance();
  void _initDebugMessenger();
  void _initPhysicalDevice();
  void _initLogicalDevice();
  void _initQueue();
  void _initSurface();

public:
  Application(const char *title);
  ~Application();

  int run();
};
} // namespace game::engine
