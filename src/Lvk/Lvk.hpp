#ifndef _VLK_HPP
#define _VLK_HPP

// Load the Vulkan header
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #include "../Window/Window.hpp"

// #include <map>
// #include <memory>

namespace lvk {
#define msw std::map<std::string, std::unique_ptr<window::Window>>

class Lvk {
public:
  Lvk();

private:
  // Initialize GLFW
  void init_glfw();

  // Initialize Vulkan
  void init_vulkan();

  //
  void create_instance();
  //
  void create_debug_messenger();
  //
  void create_surface();
  //
  void pick_physical_device();
  //
  void create_logical_device();
  //

private:
  // Instance is the connection between your application and the Vulkan
  VkInstance instance;
  // Debug messenger is used to receive debug messages from the Vulkan
  VkDebugUtilsMessengerEXT debug_messenger;
  // Surface is the connection between the window system and the instance
  VkSurfaceKHR surface;

  /** Device that is used for the rendering */
  // physical device to interface with the hardware
  VkPhysicalDevice physical_device = VK_NULL_HANDLE;
  // logical device to interface with the `physical device`
  VkDevice device;

  /** Queues selected of the logical device */
  // Queue of the present device
  VkQueue present_queue;
  // Queue of the logical device
  VkQueue graphics_queue;

  /** Swap chain */
  // The general purpose of the swap chain is to synchronize the presentation of
  // images with the refresh rate of the screen

public:
  // Run the application
  void run();
  // Destroy the vulkan resources and the GLFW
  ~Lvk();

private:
  // Clean up the resources (GLFW and Vulkan)
  void clean_up();

  // Fixed window until the implementation of multiple windows
  GLFWwindow *window;

  //   /** Windows system */
  // public:
  //   void add_window(std::string name, uint32_t width, uint32_t height);
  //   void remove_window(const std::string &name);

  // private:
  //   std::map<std::string, std::unique_ptr<window::Window>> windows;
};
} // namespace lvk

#endif
