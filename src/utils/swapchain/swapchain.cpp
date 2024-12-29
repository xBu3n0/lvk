#include "swapchain.hpp"
#include <algorithm>
#include <limits>

namespace utils {
namespace swapchain {
SwapChainSupportDetails query_swap_chain_support(VkSurfaceKHR surface,
                                                 VkPhysicalDevice device) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  /** Formats */
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         details.formats.data());
  }

  /** Present mode */
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                            &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, details.present_modes.data());
  }

  return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats) {
  // Fix variables
  // (https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain)
  for (const auto &surface_format : available_formats) {
    if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return surface_format;
    }
  }

  // If the desired format is not available, return the first one
  return available_formats[0];
}

VkPresentModeKHR choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  // Fix variables
  // (https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain)
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities, int width, int height) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}
} // namespace swapchain
} // namespace utils
