#ifndef _VLK_HPP
#define _VLK_HPP

// Load the Vulkan header
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lvk {
class Lvk {
public:
  Lvk();

private:
  // Initialize GLFW
  void init_glfw();

  // Initialize Vulkan
  void init_vulkan();

  // TODO: improve documentation
  void create_instance();
  // TODO: improve documentation
  void create_debug_messenger();
  // TODO: improve documentation
  void create_surface();
  // TODO: improve documentation
  void pick_physical_device();
  // TODO: improve documentation
  void create_logical_device();
  // TODO: improve documentation
  void create_swap_chain();
  // TODO: improve documentation
  void create_image_views();
  // TODO: improve documentation
  VkShaderModule create_shader_module(const std::vector<char> &code);
  //
  void create_render_pass();
  //
  void create_graphics_pipeline();
  //
  void create_framebuffers();
  //
  void create_command_pool();
  //
  void create_command_buffers();
  // TODO: improve documentation
  void record_command_buffer(VkCommandBuffer command_buffer,
                             uint32_t image_index);
  //
  void create_sync_objects();

private:
  /** Instance of the application */
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
  // The primary purpose of the swap chain is to synchronize the presentation of
  // images with the screen's refresh rate and to configure the format and
  // color of the images.
  VkSwapchainKHR swap_chain;

  // Store the handlers
  std::vector<VkImage> swap_chain_images;
  std::vector<VkImageView> swap_chain_image_views;

  VkFormat swap_chain_image_format;
  VkExtent2D swap_chain_extent;

  VkRenderPass render_pass;
  VkPipelineLayout pipeline_layout;
  VkPipeline graphics_pipeline;

  std::vector<VkFramebuffer> swap_chain_framebuffers;

  VkCommandPool command_pool;
  std::vector<VkCommandBuffer> command_buffers;

  std::vector<VkSemaphore> image_available_semaphore;
  std::vector<VkSemaphore> render_finished_semaphore;
  std::vector<VkFence> in_flight_fence;

public:
  // Run the application
  void run();
  // Draw the frame
  void draw_frame();
  // Destroy the vulkan resources and the GLFW
  ~Lvk();

private:
  // Clean up the resources (GLFW and Vulkan)
  void clean_up();

  // Fixed window until the implementation of multiple windows
  GLFWwindow *window;
};
} // namespace lvk

#endif
