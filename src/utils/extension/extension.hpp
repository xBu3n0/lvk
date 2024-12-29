#ifndef EXTENSION_HPP
#define EXTENSION_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <string>
#include <vector>

namespace utils {
namespace extension {
void get_extensions();
std::set<std::string> get_device_extensions(VkPhysicalDevice physical_device);
std::vector<const char *> get_window_extensions();

bool check_extensions(const std::vector<const char *> &extensions);
bool check_device_extensions(
    const std::set<std::string> &device_extensions_available,
    const std::vector<const char *> &extensions);
} // namespace extension
} // namespace utils

#endif
