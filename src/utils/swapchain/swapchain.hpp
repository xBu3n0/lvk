#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

namespace utils {
namespace swapchain {
SwapChainSupportDetails query_swap_chain_support(VkSurfaceKHR surface,
                                                 VkPhysicalDevice device);
VkSurfaceFormatKHR choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats);

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
} // namespace swapchain
} // namespace utils

#endif
