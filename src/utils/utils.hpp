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
namespace extension {
void get_extensions();
std::vector<const char *> get_glfw_extensions();

bool has_extensions(std::vector<const char *> extensions, const size_t count);

} // namespace extension

namespace messenger {
VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);
} // namespace messenger

namespace layer {
void get_layers();
std::vector<const char *> get_validation_layers();

bool check_validation_layers(std::vector<const char *> &validation_layers);
} // namespace layer

namespace queue {
QueueFamilyIndices find_queue_families(VkPhysicalDevice device,
                                       VkSurfaceKHR surface);

} // namespace queue

namespace device {
bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
} // namespace device
} // namespace utils

#endif
