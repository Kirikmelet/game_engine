#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace game::engine::renderer {

class VulkanRenderer {
private:
  vk::ApplicationInfo vkAppInfo_;
  vk::Instance vkInstance_;
public:
  VulkanRenderer();
  ~VulkanRenderer();
};

} // namespace game::engine::renderer
