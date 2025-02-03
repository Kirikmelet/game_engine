#pragma once

#include <optional>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
namespace game::engine {
#ifdef NDEBUG
const bool kEnableValidationLayer = false;
#else
const bool kEnableValidationLayer = true;
#endif

const std::vector<const char *> kVkValidationLayers = {
    "VK_LAYER_KHRONOS_validation"};

bool CheckValidationLayerSupport();
namespace debug {
VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
              VkDebugUtilsMessageTypeFlagsEXT message_type,
              const VkDebugUtilsMessengerCallbackDataEXT *message_callback_data,
              void *message_user_data);
VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo();
std::optional<VkDebugUtilsMessengerEXT>
CreateDebugMessenger(VkInstance instance,
                     const VkAllocationCallbacks *allocator,
                     VkDebugUtilsMessengerEXT &create_info);
void DestroyDebugMessenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT messenger,
                           const VkAllocationCallbacks *allocator);
} // namespace debug
} // namespace game::engine
