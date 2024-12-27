#include "Lvk.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <stack>
#include <vector>
#include <vulkan/vulkan_core.h>

std::set<std::string> extensions_available;
std::set<std::string> layers_available;

/** Validation layers */
#ifdef NDEBUG
const bool enable_validation_layer = false;
#else
const bool enable_validation_layer = true;
#endif

/** UTILS */
namespace utils {
void get_extensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  for (const auto &extension : extensions) {
    extensions_available.insert(extension.extensionName);
  }
}

void get_layers() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const auto &layer : layers) {
    layers_available.insert(layer.layerName);
  }
}

bool check_validation_layers(std::vector<const char *> &validation_layers) {
  for (const auto &layer : validation_layers) {
    if (layers_available.find(layer) == layers_available.end()) {
      return false;
    }
  }

  return true;
}

std::vector<const char *> get_glfw_extensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  // Return the extensions that glfw needs to work with Vulkan
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enable_validation_layer) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

std::vector<const char *> get_validation_layers() {
  std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

  return validation_layers;
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_ext(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}
} // namespace utils

/** VLK */
namespace lvk {
bool has_extensions(std::vector<const char *> extensions, const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (extensions_available.find(extensions[i]) ==
        extensions_available.end()) {
      return false;
    }
  }

  return true;
}

Lvk::Lvk() {
  Lvk::init();

  utils::get_extensions();
  utils::get_layers();

  std::cout << "Lvk::init_vulkan()" << std::endl;
  Lvk::init_vulkan();
}

void Lvk::init() {
  if (!glfwInit()) {
    exit(1);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Lvk::init_vulkan() {
  Lvk::create_instance();
  Lvk::create_debug_messenger();
}

void Lvk::create_instance() {
  /** Create the Application Info (optional) */
  std::cout << "appInfo" << std::endl;
  VkApplicationInfo appInfo{};

  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Lvk: Little VulKan";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.pEngineName = "VLK";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  /** Create the instance info */
  std::cout << "createInfo" << std::endl;
  VkInstanceCreateInfo createInfo{};

  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  std::vector<const char *> extensions = utils::get_glfw_extensions();

  if (!has_extensions(extensions, extensions.size())) {
    std::cerr << "Missing extensions" << std::endl;
    exit(1);
  }

  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  // Global validation layers enabled
  std::vector<const char *> validation_layers = utils::get_validation_layers();

  if (enable_validation_layer) {
    std::cout << "Validation layers enabled" << std::endl;

    if (!utils::check_validation_layers(validation_layers)) {
      std::cerr << "Missing validation layers" << std::endl;
      exit(1);
    }

    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    createInfo.ppEnabledLayerNames = validation_layers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  /** Create the instance */
  std::cout << "createInstance" << std::endl;

  VkResult result = vkCreateInstance(&createInfo, nullptr, &this->instance);

  std::cout << "result: " << result << std::endl;
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan instance" << std::endl;
    exit(1);
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  std::cerr << "Message severity: " << messageSeverity << std::endl;
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void Lvk::create_debug_messenger() {
  if (!enable_validation_layer)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;

  if (utils::create_debug_utils_messenger_ext(instance, &createInfo, nullptr,
                                              &this->debug_messenger) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void Lvk::add_window(std::string name, uint32_t width, uint32_t height) {
  this->windows.insert(
      {name, std::make_unique<window::Window>(name, width, height)});
}

void Lvk::remove_window(const std::string &name) { this->windows.erase(name); }

void Lvk::set_background_color(const std::string &name,
                               float background_color[4]) {
  this->windows[name]->set_background_color(background_color);
}

void Lvk::run() {
  std::stack<std::string> to_remove;

  while (!this->windows.empty()) {
    for (const auto &window : this->windows) {
      window.second->update();

      if (window.second->should_close()) {
        to_remove.push(window.first);
      }
    }

    while (!to_remove.empty()) {
      this->remove_window(to_remove.top());
      to_remove.pop();
    }
  }
}

void Lvk::clean_up() {
  if (enable_validation_layer) {
    utils::destroy_debug_utils_messenger_ext(this->instance,
                                             this->debug_messenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);

  // Finalize the windows
  this->windows.clear();

  glfwTerminate();
}

Lvk::~Lvk() { this->clean_up(); }
} // namespace lvk
