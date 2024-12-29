#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool is_complete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};

namespace utils {
namespace queue {
QueueFamilyIndices find_queue_families(VkPhysicalDevice device,
                                       VkSurfaceKHR surface);
} // namespace queue
} // namespace utils

#endif
