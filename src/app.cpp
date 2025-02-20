#include "app.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_vulkan.h>
#include <cassert>
#include <cstring>
#include <memory>
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
  window_ = std::make_shared<Window>(title, 480, 600);
  renderer_ = renderer::VulkanRenderer(renderer::VulkanRendererCreateInfo{
      .applicationName_ = "lol", .version_ = 1, .window_ = window_});
  SDL_zero(event_);
}

Application::~Application() {
  SDL_Quit();
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
