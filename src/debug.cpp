#include "debug.hpp"
#include <SDL3/SDL_log.h>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace game::engine {
bool CheckValidationLayerSupport() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_properties(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_properties.data());
  for (const char *layer_name : kVkValidationLayers) {
    bool layer_found = false;
    for (const VkLayerProperties &layer_property : available_properties) {
      if (strcmp(layer_name, layer_property.layerName) == 0) {
        layer_found = true;
      }
    }
    if (!layer_found) {
      return false;
    }
  }
  return true;
}
namespace debug {
VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
              VkDebugUtilsMessageTypeFlagsEXT message_type,
              const VkDebugUtilsMessengerCallbackDataEXT *message_callback_data,
              void *message_user_data) {
  SDL_Log("%s", message_callback_data->pMessage);
  return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() {
  return VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debug::DebugCallback,
  };
}
std::optional<VkDebugUtilsMessengerEXT>
CreateDebugMessenger(VkInstance instance,
                     const VkAllocationCallbacks *allocator,
                     VkDebugUtilsMessengerCreateInfoEXT &create_info) {
  PFN_vkCreateDebugUtilsMessengerEXT function =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  VkDebugUtilsMessengerEXT messenger;
  if (function == nullptr &&
      function(instance, &create_info, allocator, &messenger) != VK_SUCCESS) {
    return std::nullopt;
  } else
    return messenger;
}
void DestroyDebugMessenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT messenger,
                           const VkAllocationCallbacks *allocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT function =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
  if (function != nullptr) {
    function(instance, messenger, allocator);
  }
}
} // namespace debug
} // namespace game::engine
