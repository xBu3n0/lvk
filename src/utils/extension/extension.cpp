#include "extension.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::set<std::string> extensions_available;

/** It's not the ideal form, but it works for now */
#ifdef NDEBUG
const bool enable_validation_layer = false;
#else
const bool enable_validation_layer = true;
#endif

namespace utils {
namespace extension {
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
std::set<std::string> get_device_extensions(VkPhysicalDevice physical_device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &extensionCount, nullptr);

  std::set<std::string> device_extensions;

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &extensionCount, availableExtensions.data());

  for (const auto &extension : availableExtensions) {
    device_extensions.insert(extension.extensionName);
  }

  return device_extensions;
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

bool check_extensions(const std::vector<const char *> &extensions) {
  for (size_t i = 0; i < extensions.size(); ++i) {
    if (extensions_available.find(extensions[i]) ==
        extensions_available.end()) {
      std::cerr << "\n"
                << extensions[i] << " not available" << "\n"
                << std::endl;
      return false;
    }
  }

  return true;
}

bool check_device_extensions(
    const std::set<std::string> &device_extensions_available,
    const std::vector<const char *> &extensions) {
  for (size_t i = 0; i < extensions.size(); ++i) {
    if (device_extensions_available.find(extensions[i]) ==
        device_extensions_available.end()) {
      std::cerr << "\n"
                << extensions[i] << " not available" << "\n"
                << std::endl;
      return false;
    }
  }

  return true;
}
} // namespace extension
} // namespace utils
