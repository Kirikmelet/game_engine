#include "app.hpp"
#include "debug.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace game::engine {

Application::Application(const char *title) {

  vkAppInfo_ = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = title,
      .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
      .pEngineName = "engine",
      .apiVersion = VK_API_VERSION_1_3,
  };

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window_ = std::shared_ptr<Window>(_InitWindow(title, vkAppInfo_));
  if (!window_->IsValid()) {
    throw -1;
  }
  vkInstance_ = _initVkInstance();
  SDL_zero(event_);
}

Application::~Application() { SDL_Quit(); }

Window *Application::_InitWindow(const char *title,
                                 VkApplicationInfo &vkAppInfo) {
  return new Window(title, 680, 480);
}

VkInstance Application::_initVkInstance() {
  VkInstance instance;
  uint32_t extension_count = 0;
  char const *const *sdl_vulkan_extension_names = 0;
  sdl_vulkan_extension_names =
      SDL_Vulkan_GetInstanceExtensions(&extension_count);
  std::vector<const char *> extension_names(
      sdl_vulkan_extension_names, sdl_vulkan_extension_names + extension_count);
  extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  if (kEnableValidationLayer && CheckValidationLayerSupport()) {
    extension_names.insert(extension_names.end(), kVkValidationLayers.begin(),
                           kVkValidationLayers.end());
  }
  for (const char *name : extension_names) {
    SDL_Log("%s", name);
  }
  VkDebugUtilsMessengerEXT debugInfo = {};
  VkInstanceCreateInfo initial_vkInstanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = debugInfo,
      .enabledLayerCount = 0,
      .enabledExtensionCount = static_cast<uint32_t>(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data(),
  };
  VkResult vkResult =
      vkCreateInstance(&initial_vkInstanceCreateInfo, nullptr, &instance);
  return instance;
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
