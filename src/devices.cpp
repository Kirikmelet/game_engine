#include "devices.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <ranges>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

using namespace game::engine;

std::vector<vk::PhysicalDevice>
devices::GetPhysicalDevices(const vk::Instance &instance) {
  return instance.enumeratePhysicalDevices();
}

std::optional<vk::PhysicalDevice>
devices::FindDevice(std::vector<vk::PhysicalDevice> devices,
                    std::function<bool(vk::PhysicalDevice)> func) {
  auto iter = std::find_if(devices.begin(), devices.end(), func);
  if (iter == devices.end()) {
    return std::nullopt;
  }
  return *iter;
}

QueueFamilyIndicies
QueueFamilyIndicies::FindQueueFamily(vk::PhysicalDevice device,
                                     vk::SurfaceKHR surface) {
  QueueFamilyIndicies queueFamilyIndicies = {};
  for (auto &&[idx, property] :
       device.getQueueFamilyProperties() | std::views::enumerate) {
    vk::Bool32 supportsPresent = device.getSurfaceSupportKHR(idx, surface);
    if ((property.queueFlags & vk::QueueFlagBits::eGraphics) &&
        supportsPresent) {
      queueFamilyIndicies.graphicsfamily_ = idx;
      queueFamilyIndicies.presentFamily_ = idx;
    }
  }
  return queueFamilyIndicies;
}

bool QueueFamilyIndicies::IsReady() {
  return graphicsfamily_.has_value() && presentFamily_.has_value();
}
