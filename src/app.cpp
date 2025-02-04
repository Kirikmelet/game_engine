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
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace game::engine {

Application::Application(const char *title) {

  // vkappinfo_ = {
  //     .stype = vk_structure_type_application_info,
  //     .pnext = nullptr,
  //     .papplicationname = title,
  //     .applicationversion = vk_make_version(0, 0, 1),
  //     .penginename = "engine",
  //     .apiversion = vk_api_version_1_3,
  // };
  vkAppInfo_ = vk::ApplicationInfo{title,
                                   VK_MAKE_VERSION(0, 0, 1),
                                   "engine",
                                   VK_MAKE_VERSION(0, 0, 1),
                                   VK_API_VERSION_1_3,
                                   nullptr};
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window_ = std::shared_ptr<Window>(_InitWindow(title, vkAppInfo_));
  if (!window_->IsValid()) {
    throw -1;
  }
  try {
    vkInstance_ = _initVkInstance(vkAppInfo_);
  } catch (const vk::SystemError &error) {
    SDL_Log("%i: %s - %s", error.code().value(), error.code().message().c_str(),
            error.what());
    exit(-1);
  }
  vkDldy_ = vk::DispatchLoaderDynamic(vkInstance_, vkGetInstanceProcAddr);
  auto maybe_messenger = _initDebugMessenger(vkInstance_, vkDldy_);
  if (maybe_messenger.has_value()) {
    vkDebugMessenger_ = maybe_messenger.value();
  } else {
    SDL_Log("%s", "No debugger?");
  }
  SDL_zero(event_);
}

Application::~Application() {
  if (kEnableValidationLayer) {
    debug::DestroyDebugMessenger(vkInstance_, vkDebugMessenger_, nullptr, vkDldy_);
  }
  vkInstance_.destroy();
  SDL_Quit();
}

Window *Application::_InitWindow(const char *title,
                                 vk::ApplicationInfo &vkAppInfo) {
  return new Window(title, 680, 480);
}

VkInstance Application::_initVkInstance(vk::ApplicationInfo &vkAppInfo) {
  uint32_t extension_count = 0;
  char const *const *sdl_vulkan_extension_names = 0;
  sdl_vulkan_extension_names =
      SDL_Vulkan_GetInstanceExtensions(&extension_count);
  std::vector<const char *> extension_names(
      sdl_vulkan_extension_names, sdl_vulkan_extension_names + extension_count);
  std::vector<const char *> enabled_layers;
  extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  if (kEnableValidationLayer && CheckValidationLayerSupport()) {
    enabled_layers.insert(enabled_layers.end(), kVkValidationLayers.begin(),
                          kVkValidationLayers.end());
    // Enable debug util
    extension_names.push_back(vk::EXTDebugUtilsExtensionName);
  }
  for (const char *name : extension_names) {
    SDL_Log("%s", name);
  }
  auto debug_create_info = debug::GetDebugCreateInfo();
  vk::InstanceCreateInfo initial_vkInstanceCreateInfo(
      {}, &vkAppInfo, static_cast<uint32_t>(enabled_layers.size()),
      enabled_layers.data(), static_cast<uint32_t>(extension_names.size()),
      extension_names.data(),
      reinterpret_cast<vk::DebugUtilsMessengerCreateInfoEXT *>(
          &debug_create_info));
  // VkResult vkResult =
  //     vkCreateInstance(&initial_vkInstanceCreateInfo, nullptr, &instance);
  return vk::createInstance(initial_vkInstanceCreateInfo);
}

std::optional<vk::DebugUtilsMessengerEXT> Application::_initDebugMessenger(vk::Instance instance, vk::DispatchLoaderDynamic dldy) {
  if (!kEnableValidationLayer) {
    return std::nullopt;
  }
 vk::DebugUtilsMessengerCreateInfoEXT debug_create_info = debug::GetDebugCreateInfo();
 return debug::CreateDebugMessenger(instance, nullptr, debug_create_info, dldy);
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
