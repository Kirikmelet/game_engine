#include "renderer.hpp"
#include "debug.hpp"
#include "devices.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

using namespace game::engine::renderer;

VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo createInfo) {
  appInfo_ = vk::ApplicationInfo(
      createInfo.applicationName_, VK_MAKE_VERSION(createInfo.version_, 0, 0),
      kEngineName, VK_MAKE_VERSION(0, 0, 1), vk::ApiVersion13);
  window_ = createInfo.window_;
  _InitInstance();
  _InitDebugMessenger();
  _InitPhysicalDevices();
  _InitSurface();
  _InitLogicalDevice();
}

void VulkanRenderer::_InitInstance() {
  uint32_t sdlVulkanExtensionCount = 0;
  char const *const *sdlVulkanExtensionNames = 0;
  sdlVulkanExtensionNames =
      SDL_Vulkan_GetInstanceExtensions(&sdlVulkanExtensionCount);
  extensionNames_ =
      std::vector(sdlVulkanExtensionNames,
                  sdlVulkanExtensionNames + sdlVulkanExtensionCount);
  if (kEnableValidationLayer && CheckValidationLayerSupport()) {
    enabledLayers_.insert(enabledLayers_.end(), kVkValidationLayers.begin(),
                          kVkValidationLayers.end());
  }
  extensionNames_.push_back(vk::EXTDebugUtilsExtensionName);
  auto debugCreateInfo = debug::GetDebugCreateInfo();
  vk::InstanceCreateInfo instanceCreateInfo(
      {}, &appInfo_, static_cast<uint32_t>(enabledLayers_.size()),
      enabledLayers_.data(), static_cast<uint32_t>(extensionNames_.size()),
      extensionNames_.data(), nullptr);
  if (kEnableValidationLayer) {
    instanceCreateInfo.setPNext(
        reinterpret_cast<vk::DebugUtilsMessengerCreateInfoEXT *>(
            &debugCreateInfo));
  }
  instance_ = vk::createInstance(instanceCreateInfo);
}

void VulkanRenderer::_InitDebugMessenger() {
  if (!kEnableValidationLayer) {
    SDL_Log("Debug Messenger not enabled");
    return;
  }
  vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo =
      debug::GetDebugCreateInfo();
  auto debugMessenger = debug::CreateDebugMessenger(
      instance_, nullptr, debugCreateInfo, dispatchLoaderDynamic_);
  if (debugMessenger.has_value()) {
    debugMessenger_ = debugMessenger;
  } else {
    SDL_Log("Debug messenger not available!");
  }
}

void VulkanRenderer::_InitPhysicalDevices() {
  physicalDevices_ = std::make_unique<devices::VulkanPhysicalDevices>(
      devices::VulkanPhysicalDevices(instance_));
}

void VulkanRenderer::_InitLogicalDevice() {
  const float queuePriority = 1.0f;
  auto preferredPhysicalDevice = GetPreferredPhysicalDevice();
  device_ = std::make_shared<devices::VulkanDevice>(preferredPhysicalDevice,
                                                    surface_);
  // SDL_Log("%s", preferredPhysicalDevice.getProperties().deviceName.data());
}

void VulkanRenderer::_InitSurface() {
  VkSurfaceKHR cSurface;
  SDL_Vulkan_CreateSurface(window_->GetRawWindow(), instance_, nullptr,
                           &cSurface);
  surface_ = cSurface;
}

VulkanRenderer::~VulkanRenderer() {
  instance_.destroySurfaceKHR(surface_);
  if (debugMessenger_.has_value()) {
    debug::DestroyDebugMessenger(instance_, debugMessenger_.value(), nullptr,
                                 dispatchLoaderDynamic_);
  }
  device_.reset();
  instance_.destroy();
}

vk::PhysicalDevice &VulkanRenderer::GetPreferredPhysicalDevice() {
  return physicalDevices_->physicalDevices_.rbegin()->second;
}
