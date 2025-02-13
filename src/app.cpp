#include "app.hpp"
#include "debug.hpp"
#include "devices.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_vulkan.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace game::engine {

Application::Application(const char *title) {
  vkAppInfo_ = vk::ApplicationInfo{title,
                                   VK_MAKE_VERSION(0, 0, 1),
                                   "engine",
                                   VK_MAKE_VERSION(0, 0, 1),
                                   VK_API_VERSION_1_3,
                                   nullptr};
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  _InitWindow();
  _InitVkInstance();
  vkDldy_ = vk::DispatchLoaderDynamic(vkInstance_, vkGetInstanceProcAddr);
  SDL_zero(event_);
  _initPhysicalDevice();
  _initSurface();
  vkQueueFamily_ =
      QueueFamilyIndicies::FindQueueFamily(vkPhysicalDevice_, vkSurface_);
  assert(vkQueueFamily_.IsReady());
  _initLogicalDevice();
}

Application::~Application() {
  if (kEnableValidationLayer) {
    debug::DestroyDebugMessenger(vkInstance_, vkDebugMessenger_, nullptr,
                                 vkDldy_);
  }
  vkDevice_.destroy();
  vkInstance_.destroy();
  SDL_Quit();
}

void Application::_InitWindow() {
  Window *window = new Window(title_.c_str(), 680, 480);
  assert(window->IsValid());
  window_ = std::shared_ptr<Window>(window);
}

void Application::_InitVkInstance() {
  uint32_t extension_count = 0;
  char const *const *sdl_vulkan_extension_names = 0;
  sdl_vulkan_extension_names =
      SDL_Vulkan_GetInstanceExtensions(&extension_count);
  vkExtensionNames_ = std::vector(sdl_vulkan_extension_names,
                                  sdl_vulkan_extension_names + extension_count);
  if (kEnableValidationLayer && CheckValidationLayerSupport()) {
    vkEnabledLayers_.insert(vkEnabledLayers_.end(), kVkValidationLayers.begin(),
                            kVkValidationLayers.end());
    // Enable debug util
    vkExtensionNames_.push_back(vk::EXTDebugUtilsExtensionName);
  }
  for (const char *name : vkExtensionNames_) {
    SDL_Log("%s", name);
  }
  auto debug_create_info = debug::GetDebugCreateInfo();
  vk::InstanceCreateInfo initial_vkInstanceCreateInfo(
      {}, &vkAppInfo_, static_cast<uint32_t>(vkEnabledLayers_.size()),
      vkEnabledLayers_.data(), static_cast<uint32_t>(vkExtensionNames_.size()),
      vkExtensionNames_.data(),
      reinterpret_cast<vk::DebugUtilsMessengerCreateInfoEXT *>(
          &debug_create_info));
  vkInstance_ = vk::createInstance(initial_vkInstanceCreateInfo);
}

void Application::_initDebugMessenger() {
  if (!kEnableValidationLayer) {
    return;
  }
  vk::DebugUtilsMessengerCreateInfoEXT debug_create_info =
      debug::GetDebugCreateInfo();
  std::optional<vk::DebugUtilsMessengerEXT> debugMessenger =
      debug::CreateDebugMessenger(vkInstance_, nullptr, debug_create_info,
                                  vkDldy_);
  if (debugMessenger.has_value()) {
    vkDebugMessenger_ = debugMessenger.value();
  } else {
    SDL_Log("Debug Messenger not available!");
  }
}

void Application::_initPhysicalDevice() {
  auto devices = devices::GetPhysicalDevices(vkInstance_);
  auto device = devices::FindDevice(devices, [&](vk::PhysicalDevice device) {
    return (device.getProperties().deviceType ==
                vk::PhysicalDeviceType::eDiscreteGpu &&
            device.getFeatures().geometryShader);
  });
  assert(device.has_value());
  vkPhysicalDevice_ = device.value();
}

void Application::_initLogicalDevice() {
  const float queuePriority = 1.0f;
  std::set<uint32_t> uniqueQueueFamilies = {
      vkQueueFamily_.graphicsfamily_.value(),
      vkQueueFamily_.presentFamily_.value()};
  std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
  for (auto idx : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo{
        {}, idx, 1, &queuePriority};
    deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
  }
  assert(deviceQueueCreateInfos[0].queueCount > 0);
  vk::DeviceCreateInfo deviceCreateInfo =
      vk::DeviceCreateInfo{{}, static_cast<uint32_t>(deviceQueueCreateInfos.size()), deviceQueueCreateInfos.data()};
  vkDevice_ = vkPhysicalDevice_.createDevice(deviceCreateInfo);
}
void Application::_initQueue() {
  if (!vkQueueFamily_.IsReady()) {
    return;
  }
  auto graphicsQueue =
      vkDevice_.getQueue(vkQueueFamily_.graphicsfamily_.value(), 0);
}

void Application::_initSurface() {
  VkSurfaceKHR cSurface;
  SDL_Vulkan_CreateSurface(window_->GetRawWindow(), vkInstance_, nullptr,
                           &cSurface);
  vkSurface_ = cSurface;
}

int Application::run() {
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event_)) {
      if (event_.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }
  }
  return 0;
}
} // namespace game::engine
