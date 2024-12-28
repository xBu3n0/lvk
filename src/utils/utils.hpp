#ifndef UTILS_HPP
#define UTILS_HPP

#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool is_complete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};

/** Validation layers */
#ifdef NDEBUG
const bool enable_validation_layer = false;
#else
const bool enable_validation_layer = true;
#endif

/** UTILS */
namespace utils {
void get_extensions();

bool has_extensions(std::vector<const char *> extensions, const size_t count);

void get_layers();

bool check_validation_layers(std::vector<const char *> &validation_layers);

std::vector<const char *> get_glfw_extensions();

std::vector<const char *> get_validation_layers();

QueueFamilyIndices find_queue_families(VkPhysicalDevice device,
                                       VkSurfaceKHR surface);

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void destroy_debug_utils_messenger_ext(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator);

bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
} // namespace utils

#endif
