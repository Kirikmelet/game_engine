#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>
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
DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
              vk::DebugUtilsMessageTypeFlagsEXT message_type,
              const VkDebugUtilsMessengerCallbackDataEXT *message_callback_data,
              void *message_user_data);
vk::DebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo();
std::optional<vk::DebugUtilsMessengerEXT>
CreateDebugMessenger(vk::Instance instance,
                     const vk::AllocationCallbacks *allocator,
                     vk::DebugUtilsMessengerCreateInfoEXT const &create_info,
                     vk::DispatchLoaderDynamic& dldy);
void DestroyDebugMessenger(vk::Instance instance,
                           vk::DebugUtilsMessengerEXT messenger,
                           const vk::AllocationCallbacks *allocator,
                           vk::DispatchLoaderDynamic& dldy);
// DebugCleanup()
} // namespace debug
} // namespace game::engine
