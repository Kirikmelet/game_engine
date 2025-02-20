#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace game::engine {

struct QueueFamilyIndicies {
  std::optional<uint32_t> graphicsfamily_;
  std::optional<uint32_t> presentFamily_;
  QueueFamilyIndicies();
  QueueFamilyIndicies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

  std::set<uint32_t> GetQueueFamilySet();
  bool IsReady();
};

namespace devices {

constexpr float kDeviceQueuePriority = 1.0f;

struct VulkanDevice {
  vk::Device device_;
  game::engine::QueueFamilyIndicies queueFamilyIndicies_;

  VulkanDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR);
  ~VulkanDevice();
};

struct VulkanPhysicalDevices {
  std::multimap<uint32_t, vk::PhysicalDevice> physicalDevices_;
  VulkanPhysicalDevices(const vk::Instance &instance,
                        std::function<uint32_t(const vk::PhysicalDevice &)>
                            pred = DefaultGPUSort);
  std::optional<vk::PhysicalDevice>
  QueryDevice(std::function<bool(const vk::PhysicalDevice &)> func);
  static std::multimap<uint32_t, vk::PhysicalDevice>
  SortPhysicalDevices(std::vector<vk::PhysicalDevice> devices,
                      std::function<uint32_t(const vk::PhysicalDevice &)> pred);

  static bool DiscreteGPUQuery(const vk::PhysicalDevice &device);
  static uint32_t DefaultGPUSort(const vk::PhysicalDevice &device);
};

}; // namespace devices
} // namespace game::engine
