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

private:
  // Instance is the connection between your application and the Vulkan
  VkInstance instance;
  //
  VkDebugUtilsMessengerEXT debug_messenger;
  //
  VkSurfaceKHR surface;

  //
  VkQueue present_queue;
  //
  VkPhysicalDevice physical_device = VK_NULL_HANDLE;

  // logical device to interface with the `physical device`
  VkDevice device;

  // Queue of the logical device
  VkQueue graphics_queue;

public:
  //
  void run();
  //
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
