#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace game::engine {
namespace devices {
std::vector<vk::PhysicalDevice>
GetPhysicalDevices(const vk::Instance &instance);
std::optional<vk::PhysicalDevice>
FindDevice(std::vector<vk::PhysicalDevice> devices,
           std::function<bool(vk::PhysicalDevice)> func);
}; // namespace devices

struct QueueFamilyIndicies {
  std::optional<uint32_t> graphicsfamily_;
  std::optional<uint32_t> presentFamily_;
  static QueueFamilyIndicies FindQueueFamily(vk::PhysicalDevice device, vk::SurfaceKHR surface);
  bool IsReady();
};

} // namespace game::engine
