#ifndef DEVICE_HPP
#define DEVICE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace utils {
namespace device {
bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface,
                        std::vector<const char *> &device_extensions);
} // namespace device
} // namespace utils

#endif
