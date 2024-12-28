#include "utils.hpp"

#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <string>
#include <cstdint>
#include <vector>

std::set<std::string> extensions_available;
std::set<std::string> layers_available;

extern const bool enable_validation_layer;

/** UTILS */
namespace utils {
void get_extensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  for (const auto &extension : extensions) {
    extensions_available.insert(extension.extensionName);
  }
}

bool has_extensions(std::vector<const char *> extensions, const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (extensions_available.find(extensions[i]) ==
        extensions_available.end()) {
      return false;
    }
  }

  return true;
}

void get_layers() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const auto &layer : layers) {
    layers_available.insert(layer.layerName);
  }
}

bool check_validation_layers(std::vector<const char *> &validation_layers) {
  for (const auto &layer : validation_layers) {
    if (layers_available.find(layer) == layers_available.end()) {
      return false;
    }
  }

  return true;
}

std::vector<const char *> get_glfw_extensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  // Return the extensions that glfw needs to work with Vulkan
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enable_validation_layer) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

std::vector<const char *> get_validation_layers() {
  std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

  return validation_layers;
}

QueueFamilyIndices find_queue_families(VkPhysicalDevice device,
                                       VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queue_families.data());

  for (size_t i = 0; i < queue_families.size(); ++i) {
    const auto &queue_family = queue_families[i];

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

    if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support) {
      indices.graphics_family = i;
      indices.present_family = i;

      break;
    }
  }

  return indices;
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_ext(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  QueueFamilyIndices indices = utils::find_queue_families(device, surface);

  return indices.is_complete();
}
} // namespace utils
