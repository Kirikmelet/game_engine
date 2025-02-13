#include "debug.hpp"
#include <SDL3/SDL_log.h>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace game::engine {
bool CheckValidationLayerSupport() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<vk::LayerProperties> available_properties(layer_count);
  vk::Result vkResult1 = vk::enumerateInstanceLayerProperties(
      &layer_count, available_properties.data());
  for (const char *layer_name : kVkValidationLayers) {
    bool layer_found = false;
    for (const vk::LayerProperties &layer_property : available_properties) {
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
VKAPI_ATTR uint32_t VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
              VkDebugUtilsMessageTypeFlagsEXT message_type,
              VkDebugUtilsMessengerCallbackDataEXT const *message_callback_data,
              void *message_user_data) {
  SDL_Log("%s", message_callback_data->pMessage);
  return VK_FALSE;
}

vk::DebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo() {
  return vk::DebugUtilsMessengerCreateInfoEXT{
      {},
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      &debug::DebugCallback};
}
std::optional<vk::DebugUtilsMessengerEXT>
CreateDebugMessenger(vk::Instance instance,
                     const vk::AllocationCallbacks *allocator,
                     vk::DebugUtilsMessengerCreateInfoEXT const &create_info,
                     vk::DispatchLoaderDynamic& dldy) {
  PFN_vkCreateDebugUtilsMessengerEXT function =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
  if (function != nullptr) {
  dldy.init(instance, vkGetInstanceProcAddr);
    vk::DebugUtilsMessengerEXT messenger =
        instance.createDebugUtilsMessengerEXT(create_info, allocator, dldy);
    return messenger;
  }
  return std::nullopt;
}
void DestroyDebugMessenger(vk::Instance instance,
                           vk::DebugUtilsMessengerEXT messenger,
                           const vk::AllocationCallbacks *allocator,
                           vk::DispatchLoaderDynamic& dldy) {
  PFN_vkDestroyDebugUtilsMessengerEXT function =
      reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
  dldy.init(instance, vkGetInstanceProcAddr);
  if (function != nullptr) {
    instance.destroyDebugUtilsMessengerEXT(messenger, allocator, dldy);
  }
}
} // namespace debug
} // namespace game::engine
