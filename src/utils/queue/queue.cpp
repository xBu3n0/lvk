#include "queue.hpp"

#include <vector>

namespace utils {
namespace queue {
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

    if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      indices.graphics_family = i;
    }

    if (present_support) {
      indices.present_family = i;
    }

    if (indices.is_complete()) {
      break;
    }
  }

  return indices;
}
} // namespace queue
} // namespace utils
