#include "Lvk.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../utils/utils.hpp"

/** Validation layers */
extern const bool enable_validation_layer;

/** VLK */
namespace lvk {
Lvk::Lvk() {
  Lvk::init_glfw();

  utils::get_extensions();
  utils::get_layers();

  std::cout << "Lvk::init_vulkan()" << std::endl;
  Lvk::init_vulkan();
}

void Lvk::init_glfw() {
  if (!glfwInit()) {
    exit(1);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  this->window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void Lvk::init_vulkan() {
  Lvk::create_instance();
  Lvk::create_debug_messenger();
  Lvk::create_surface();
  Lvk::pick_physical_device();
  Lvk::create_logical_device();
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

  if (!utils::has_extensions(extensions, extensions.size())) {
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

void Lvk::create_surface() {
  if (glfwCreateWindowSurface(this->instance, window, nullptr,
                              &this->surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void Lvk::pick_physical_device() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (utils::is_device_suitable(device, this->surface)) {
      std::cout << "Device " << device << " suitable" << std::endl;
      this->physical_device = device;
      break;
    }
  }

  if (this->physical_device == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

void Lvk::create_logical_device() {
  QueueFamilyIndices indices =
      utils::find_queue_families(this->physical_device, this->surface);

  /** Describes the number of queues we want for a single queue family */
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphics_family.value();
  queueCreateInfo.queueCount = 1;

  float queue_priority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queue_priority;

  /** Specify is the set of device features that we'll be using. */
  VkPhysicalDeviceFeatures device_features{};

  /** Create the logical device */
  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.pQueueCreateInfos = &queueCreateInfo;
  create_info.queueCreateInfoCount = 1;

  create_info.pEnabledFeatures = &device_features;

  /** Skip device specific validation layers (Citar REF) */
  //

  if (vkCreateDevice(this->physical_device, &create_info, nullptr,
                     &this->device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  /** Get the queue of the created device */
  vkGetDeviceQueue(device, indices.graphics_family.value(), 0,
                   &this->graphics_queue);
}

// void Lvk::add_window(std::string name, uint32_t width, uint32_t height) {
//   this->windows.insert(
//       {name, std::make_unique<window::Window>(name, width, height)});
// }

// void Lvk::remove_window(const std::string &name) { this->windows.erase(name);
// }

void Lvk::run() {
  while (!glfwWindowShouldClose(this->window)) {
    glfwPollEvents();
  }
}

// void Lvk::run() {
//   std::stack<std::string> to_remove;

//   while (!this->windows.empty()) {
//     for (const auto &window : this->windows) {
//       window.second->update();

//       if (window.second->should_close()) {
//         to_remove.push(window.first);
//       }
//     }

//     while (!to_remove.empty()) {
//       this->remove_window(to_remove.top());
//       to_remove.pop();
//     }
//   }
// }

void Lvk::clean_up() {
  vkDestroyDevice(this->device, nullptr);

  vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

  if (enable_validation_layer) {
    utils::destroy_debug_utils_messenger_ext(this->instance,
                                             this->debug_messenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);

  // Finalize the windows
  // this->windows.clear();
  glfwDestroyWindow(this->window);

  glfwTerminate();
}

Lvk::~Lvk() { this->clean_up(); }
} // namespace lvk
