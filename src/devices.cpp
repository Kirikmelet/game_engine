#include "devices.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <ranges>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

using namespace game::engine;

namespace game::engine::devices {

VulkanPhysicalDevices::VulkanPhysicalDevices(
    const vk::Instance &instance,
    std::function<
        uint32_t(const vk::PhysicalDevice &)>
        pred) {
  auto devices = instance.enumeratePhysicalDevices();
  physicalDevices_ = SortPhysicalDevices(devices, pred);
}

std::optional<vk::PhysicalDevice> VulkanPhysicalDevices::QueryDevice(
    std::function<bool(const vk::PhysicalDevice &)> func) {
  auto devices = &physicalDevices_;
  auto iter = std::find_if(devices->begin(), devices->end(),
                           [&](std::pair<uint32_t, vk::PhysicalDevice> i) {
                             func(i.second);
                             return true;
                           });
  if (iter == devices->end()) {
    return std::nullopt;
  }
  return iter->second;
}

bool VulkanPhysicalDevices::DiscreteGPUQuery(const vk::PhysicalDevice &device) {
  return (device.getProperties().deviceType ==
              vk::PhysicalDeviceType::eDiscreteGpu &&
          device.getFeatures().geometryShader);
}

uint32_t
VulkanPhysicalDevices::DefaultGPUSort(const vk::PhysicalDevice &device) {
  size_t score = 0;
  auto properties = device.getProperties();
  switch (properties.deviceType) {
  case (vk::PhysicalDeviceType::eDiscreteGpu):
    score += 1000;
  case (vk::PhysicalDeviceType::eIntegratedGpu):
    score += 750;
  case (vk::PhysicalDeviceType::eCpu):
    score += 500;
  case (vk::PhysicalDeviceType::eVirtualGpu):
    score += 250;
  case (vk::PhysicalDeviceType::eOther):
    score += 100;
  default:
    break;
  }
  return score;
}
std::multimap<uint32_t, vk::PhysicalDevice>
VulkanPhysicalDevices::SortPhysicalDevices(
    std::vector<vk::PhysicalDevice> devices,
    std::function<uint32_t(const vk::PhysicalDevice &)> pred) {
  std::multimap<uint32_t, vk::PhysicalDevice> sortedMap;
  for (const auto &i : devices) {
    sortedMap.insert(std::pair(pred(i), i));
  }
  return sortedMap;
}

VulkanDevice::VulkanDevice(vk::PhysicalDevice physicalDevice,
                           vk::SurfaceKHR surface) {
  queueFamilyIndicies_ = QueueFamilyIndicies(physicalDevice, surface);
  auto uniqueIndicies = queueFamilyIndicies_.GetQueueFamilySet();
  std::vector<vk::DeviceQueueCreateInfo> uniqueCreateInfos;
  uniqueCreateInfos.reserve(uniqueIndicies.size());
  for (auto idx : uniqueIndicies) {
    vk::DeviceQueueCreateInfo createInfo{{}, idx, 1, &kDeviceQueuePriority};
    uniqueCreateInfos.push_back(createInfo);
  }
  auto deviceCreateInfo =
      vk::DeviceCreateInfo{{},
                           static_cast<uint32_t>(uniqueCreateInfos.size()),
                           uniqueCreateInfos.data()};
  device_ = physicalDevice.createDevice(deviceCreateInfo);
}

VulkanDevice::~VulkanDevice() {
  device_.destroy();
}

}; // namespace game::engine::devices

QueueFamilyIndicies::QueueFamilyIndicies() {
  graphicsfamily_ = std::nullopt;
  presentFamily_ = std::nullopt;
}

QueueFamilyIndicies::QueueFamilyIndicies(vk::PhysicalDevice device,
                                         vk::SurfaceKHR surface) {
  for (auto &&[idx, property] :
       device.getQueueFamilyProperties() | std::views::enumerate) {
    vk::Bool32 supportsPresent = device.getSurfaceSupportKHR(idx, surface);
    if ((property.queueFlags & vk::QueueFlagBits::eGraphics) &&
        supportsPresent) {
      graphicsfamily_ = idx;
      presentFamily_ = idx;
    }
  };
}

bool QueueFamilyIndicies::IsReady() {
  return graphicsfamily_.has_value() && presentFamily_.has_value();
}

std::set<uint32_t> QueueFamilyIndicies::GetQueueFamilySet() {
  if (!IsReady()) {
    return {};
  } else {
    return {graphicsfamily_.value(), presentFamily_.value()};
  }
}
