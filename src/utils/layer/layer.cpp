#include "layer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <set>
#include <string>

std::set<std::string> layers_available;

namespace utils {
namespace layer {
void get_layers() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const auto &layer : layers) {
    layers_available.insert(layer.layerName);
  }
}
std::vector<const char *> get_validation_layers() {
  std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

  return validation_layers;
}

bool check_validation_layers(
    const std::vector<const char *> &validation_layers) {
  for (const auto &layer : validation_layers) {
    if (layers_available.find(layer) == layers_available.end()) {
      return false;
    }
  }

  return true;
}
} // namespace layer
} // namespace utils
