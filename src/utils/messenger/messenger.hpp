#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace utils {
namespace messenger {
VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void destroy_debug_utils_messenger_ext(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);
} // namespace messenger
} // namespace utils

#endif
