#include "device.hpp"

#include "../extension/extension.hpp"
#include "../queue/queue.hpp"
#include "../swapchain/swapchain.hpp"

#include <iostream>
#include <set>
#include <string>

namespace utils {
namespace device {
bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface,
                        std::vector<const char *> &device_extensions) {
  /** Hardware specifications */
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  /** Device extension support */
  std::set<std::string> available_device_extensions =
      utils::extension::get_device_extensions(device);

  /** Graphics family and Presentation family */
  QueueFamilyIndices indices = queue::find_queue_families(device, surface);
  bool extension_supported = extension::check_device_extensions(
      available_device_extensions, device_extensions);

  // Physical device extensions
  std::cout << "Device: " << device_properties.deviceName << std::endl;
  std::cout << "Device extensions: " << std::endl;
  for (const auto &extension : available_device_extensions) {
    std::cout << "\t" << extension << std::endl;
  }

  // Required extensions
  std::cout << "Extensions required:" << std::endl;
  for (const auto &extension : device_extensions) {
    std::cout << "\t" << extension << std::endl;
  }

  // Check if the device support the extensions
  std::cout << "Extensions supported? " << extension_supported << std::endl;

  /** Swap chain support (Verify only if has the swapchain extension) */
  bool swap_chain_adequate = false;
  if (extension_supported) {
    SwapChainSupportDetails swap_chain_support =
        utils::swapchain::query_swap_chain_support(surface, device);

    swap_chain_adequate = !swap_chain_support.formats.empty() &&
                          !swap_chain_support.present_modes.empty();
  }

  return indices.is_complete() && extension_supported && swap_chain_adequate;
}
} // namespace device
} // namespace utils
