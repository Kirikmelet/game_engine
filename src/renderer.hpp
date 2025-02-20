#pragma once
#include "devices.hpp"
#include "window.hpp"
#include <cstddef>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace game::engine::renderer {

class RendererBase {};

constexpr const char *kEngineName = "Kirikmelet's Engine";

struct VulkanRendererCreateInfo {
  const char *applicationName_;
  const int version_;
  std::shared_ptr<Window> window_;
};

class VulkanRenderer : public RendererBase {
private:
  std::shared_ptr<Window> window_;
  vk::ApplicationInfo appInfo_;
  vk::Instance instance_;
  vk::DispatchLoaderDynamic dispatchLoaderDynamic_;
  std::unique_ptr<devices::VulkanPhysicalDevices> physicalDevices_;
  vk::PhysicalDevice preferredPhysicalDevice_;
  std::shared_ptr<devices::VulkanDevice> device_;
  std::vector<const char *> enabledLayers_;
  std::vector<const char *> extensionNames_;
  vk::SurfaceKHR surface_;
  std::vector<vk::Queue> queues_;
  std::optional<vk::DebugUtilsMessengerEXT> debugMessenger_;

  void _InitInstance();
  void _InitDebugMessenger();
  void _InitPhysicalDevices();
  void _InitLogicalDevice();
  void _InitSurface();

public:
  VulkanRenderer(const VulkanRendererCreateInfo createInfo = {});
  ~VulkanRenderer();

  vk::PhysicalDevice &GetPreferredPhysicalDevice();
  // std::optional<vk::PhysicalDevice> QueryPhysicalDevice(std::function<bool(const vk::PhysicalDevice&)> query);
};

} // namespace game::engine::renderer
