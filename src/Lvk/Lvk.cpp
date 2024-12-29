#include "Lvk.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../utils/utils.hpp"

/** Validation layers */
extern const bool enable_validation_layer;

std::vector<const char *> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/** VLK */
namespace lvk {
Lvk::Lvk() {
  // Inicializa a janela (GLFW)
  Lvk::init_glfw();

  // Inicializar as variaveis do `utils`
  utils::extension::get_extensions();
  utils::layer::get_layers();

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
  std::cout << "create_info" << std::endl;
  VkInstanceCreateInfo create_info{};

  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &appInfo;

  std::vector<const char *> extensions =
      utils::extension::get_glfw_extensions();

  if (!utils::extension::check_extensions(extensions)) {
    std::cerr << "Missing extensions" << std::endl;
    exit(1);
  }

  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  // Global validation layers enabled
  std::vector<const char *> validation_layers =
      utils::layer::get_validation_layers();

  if (enable_validation_layer) {
    std::cout << "Validation layers enabled" << std::endl;

    if (!utils::layer::check_validation_layers(validation_layers)) {
      std::cerr << "Missing validation layers" << std::endl;
      exit(1);
    }

    create_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  /** Create the instance */
  std::cout << "createInstance" << std::endl;

  VkResult result = vkCreateInstance(&create_info, nullptr, &this->instance);

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
  if (!enable_validation_layer) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debugCallback;
  create_info.pUserData = nullptr;

  if (utils::messenger::create_debug_utils_messenger_ext(
          instance, &create_info, nullptr, &this->debug_messenger) !=
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
    if (utils::device::is_device_suitable(device, this->surface,
                                          device_extensions)) {
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
      utils::queue::find_queue_families(this->physical_device, this->surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphics_family.value(),
                                            indices.present_family.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  /** Specify is the set of device features that we'll be using. */
  VkPhysicalDeviceFeatures device_features{};

  /** Create the logical device */
  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  create_info.pQueueCreateInfos = queueCreateInfos.data();

  create_info.pEnabledFeatures = &device_features;

  /** Device extension */
  // Dont need to be validated because we already checked it on
  // `is_device_suitable` when picking the physical device
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(device_extensions.size());
  create_info.ppEnabledExtensionNames = device_extensions.data();

  /** Skip device specific validation layers because is deprecated (REF) */
  //

  if (vkCreateDevice(this->physical_device, &create_info, nullptr,
                     &this->device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  /** Get the queue of the created device */
  vkGetDeviceQueue(device, indices.graphics_family.value(), 0,
                   &this->graphics_queue);
  vkGetDeviceQueue(device, indices.present_family.value(), 0,
                   &this->present_queue);

  std::cout << this->graphics_queue << std::endl;
  std::cout << this->present_queue << std::endl;
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
    utils::messenger::destroy_debug_utils_messenger_ext(
        this->instance, this->debug_messenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);

  // Finalize the windows
  // this->windows.clear();
  glfwDestroyWindow(this->window);

  glfwTerminate();
}

Lvk::~Lvk() { this->clean_up(); }
} // namespace lvk
