#include "Lvk.hpp"

#include "../utils/utils.hpp"
#include <iostream>

/** Validation layers */
extern const bool enable_validation_layer;

std::vector<const char *> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/** VLK */
namespace lvk {
Lvk::Lvk() {
  // Inicializa a janela (GLFW)
  this->init_glfw();

  // Inicializar as variaveis do `utils`
  utils::extension::get_extensions();
  utils::layer::get_layers();

  std::cout << "Lvk::init_vulkan()" << std::endl;
  this->init_vulkan();
}

void Lvk::init_glfw() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  this->window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void Lvk::init_vulkan() {
  std::cout << "Lvk::create_instance()" << std::endl;
  this->create_instance();

  std::cout << "Lvk::create_debug_messenger()" << std::endl;
  this->create_debug_messenger();

  std::cout << "Lvk::create_surface()" << std::endl;
  this->create_surface();

  std::cout << "Lvk::pick_physical_device()" << std::endl;
  this->pick_physical_device();

  std::cout << "Lvk::create_logical_device()" << std::endl;
  this->create_logical_device();

  std::cout << "Lvk::create_swap_chain()" << std::endl;
  this->create_swap_chain();

  std::cout << "Lvk::create_image_views()" << std::endl;
  this->create_image_views();

  std::cout << "Lvk::create_render_pass()" << std::endl;
  this->create_render_pass();

  std::cout << "Lvk::create_graphics_pipeline()" << std::endl;
  this->create_graphics_pipeline();

  std::cout << "Lvk::create_framebuffers()" << std::endl;
  this->create_framebuffers();

  std::cout << "Lvk::create_command_pool()" << std::endl;
  this->create_command_pool();

  std::cout << "Lvk::create_command_buffer()" << std::endl;
  this->create_command_buffer();

  std::cout << "Lvk::create_sync_objects()" << std::endl;
  this->create_sync_objects();
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
    throw std::runtime_error("Missing extensions");
  }

  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  // Global validation layers enabled
  std::vector<const char *> validation_layers =
      utils::layer::get_validation_layers();

  if (enable_validation_layer) {
    std::cout << "Validation layers enabled" << std::endl;

    if (!utils::layer::check_validation_layers(validation_layers)) {
      throw std::runtime_error("Missing validation layers");
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
    throw std::runtime_error("Failed to create Vulkan instance");
  }
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
  create_info.pfnUserCallback = utils::messenger::debugCallback;
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

void Lvk::create_swap_chain() {
  /** Creation of the swapchain */
  SwapChainSupportDetails swap_chain_support =
      utils::swapchain::query_swap_chain_support(this->surface,
                                                 this->physical_device);

  VkSurfaceFormatKHR surface_format =
      utils::swapchain::choose_swap_surface_format(swap_chain_support.formats);
  VkPresentModeKHR present_mode = utils::swapchain::choose_swap_present_mode(
      swap_chain_support.present_modes);

  int width, height;
  glfwGetWindowSize(this->window, &width, &height);

  VkExtent2D extent = utils::swapchain::choose_swap_extent(
      swap_chain_support.capabilities, width, height);

  /** How many images we would like to have in the swap chain */
  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  /** Creation of the structure */
  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface;

  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices =
      utils::queue::find_queue_families(this->physical_device, this->surface);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(),
                                     indices.present_family.value()};

  /** Handle swap chain images that will be used across multiple queue families
   */
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }

  /** To specify that you do not want any transformation, simply specify the
   * current transformation. */
  create_info.preTransform = swap_chain_support.capabilities.currentTransform;

  /** The compositeAlpha field specifies if the alpha channel should be used for
   * blending with other windows in the window system. You'll almost always want
   * to simply ignore the alpha channel, hence
   * VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR. (play with this after) */
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(this->device, &create_info, nullptr,
                           &this->swap_chain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(this->device, this->swap_chain, &image_count,
                          nullptr);

  this->swap_chain_images.resize(image_count);

  vkGetSwapchainImagesKHR(this->device, this->swap_chain, &image_count,
                          this->swap_chain_images.data());

  this->swap_chain_image_format = surface_format.format;
  this->swap_chain_extent = extent;
}

void Lvk::create_image_views() {
  this->swap_chain_image_views.resize(this->swap_chain_images.size());

  for (size_t i = 0; i < this->swap_chain_image_views.size(); i++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = this->swap_chain_images[i];

    // How the image data should be interpreted
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = this->swap_chain_image_format;

    // swizzle the color channels
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Describes what the image's purpose is and which part of the image should
    // be accessed Will be used as color targets without any mipmapping levels
    // or multiple layers
    // (https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Image_views)
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &create_info, nullptr,
                          &this->swap_chain_image_views[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

void Lvk::create_render_pass() {
  /** Attachment Description */
  VkAttachmentDescription color_attachment{};
  color_attachment.format = this->swap_chain_image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  // We don't do anything with the stencil buffer yet (don't store)
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  /** Color attachment reference */
  VkAttachmentReference color_attachment_ref{};
  // specifies which attachment to reference by its index in the attachment
  // descriptions array. (VkAttachmentDescription)
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  /** Subpass */
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  /** Render pass */
  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  if (vkCreateRenderPass(device, &render_pass_info, nullptr,
                         &this->render_pass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Lvk::create_graphics_pipeline() {
  /** Read the shaders */
  std::vector<char> vert_shader_code =
      utils::file::read_file("shaders/vert.spv");
  std::vector<char> frag_shader_code =
      utils::file::read_file("shaders/frag.spv");

  VkShaderModule vert_shader_module = create_shader_module(vert_shader_code);
  VkShaderModule frag_shader_module = create_shader_module(frag_shader_code);

  /** Create the pipeline */
  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT; // vertex shader
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage =
      VK_SHADER_STAGE_FRAGMENT_BIT; // fragment shader
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info,
                                                     frag_shader_stage_info};

  /** Input assembly */
  //
  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  /** Vertex input */
  // Fixed for now because the vertex are written directly in the shader
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
  vertex_input_info.vertexAttributeDescriptionCount = 0;
  vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

  /** Set some variables that can be dynamic in the pipeline */
  // It can be fixed or dynamic, if set to dynamic, need to be passed
  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount =
      static_cast<uint32_t>(dynamic_states.size());
  dynamic_state.pDynamicStates = dynamic_states.data();

  /** Viewport */
  /** Without dynamic state
    // Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)this->swap_chain_extent.width;
    viewport.height = (float)this->swap_chain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->swap_chain_extent;
  */
  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  // viewport_state.pViewports = &viewport; // Without dynamic state
  viewport_state.scissorCount = 1;
  // viewport_state.pScissors = &scissor; // // Without dynamic state

  /** Rasterizer */
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;

  rasterizer.rasterizerDiscardEnable = VK_FALSE;

  rasterizer.polygonMode =
      VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_FILL, VK_POLYGON_MODE_LINE,
                            // VK_POLYGON_MODE_POINT (need to enable feature for
                            // other than fill)

  rasterizer.lineWidth =
      1.0f; // If lineWidth > 1.0f, need to enable feature wideLines

  // Cull mode and front face
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

  //
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  /** Multisampling */
  // Disabled for now
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  /** Color blending */
  // Configuration per attached framebuffer
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  // No blending
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;                     // Disabled
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

  // Alpha blending
  // color_blend_attachment.blendEnable = VK_TRUE;
  // color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  // color_blend_attachment.dstColorBlendFactor =
  //     VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  // color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  // color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  // color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  // color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  // Global color blending settings
  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f; // Optional
  color_blending.blendConstants[1] = 0.0f; // Optional
  color_blending.blendConstants[2] = 0.0f; // Optional
  color_blending.blendConstants[3] = 0.0f; // Optional

  /** Pipeline layout */
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &this->pipeline_layout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  /** Pipeline */
  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  // Shader stages
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;

  // Fixed function stage
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = nullptr; // Optional
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = &dynamic_state;

  // Pipeline layout
  pipeline_info.layout = this->pipeline_layout;

  // Render pass
  pipeline_info.renderPass = this->render_pass;
  pipeline_info.subpass = 0;

  // Pipeline derivatives
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipeline_info.basePipelineIndex = -1;              // Optional

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                nullptr,
                                &this->graphics_pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  /** Destroy the shaders when the pipeline is created */
  vkDestroyShaderModule(this->device, vert_shader_module, nullptr);
  vkDestroyShaderModule(this->device, frag_shader_module, nullptr);
}

void Lvk::create_framebuffers() {
  this->swap_chain_framebuffers.resize(this->swap_chain_image_views.size());

  for (size_t i = 0; i < this->swap_chain_image_views.size(); i++) {
    VkImageView attachments[] = {this->swap_chain_image_views[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = this->render_pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = this->swap_chain_extent.width;
    framebufferInfo.height = this->swap_chain_extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &this->swap_chain_framebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Lvk::create_command_pool() {
  QueueFamilyIndices queue_family_indices =
      utils::queue::find_queue_families(this->physical_device, this->surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queue_family_indices.graphics_family.value();

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &this->command_pool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void Lvk::create_command_buffer() {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = this->command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device, &alloc_info, &this->command_buffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void Lvk::record_command_buffer(VkCommandBuffer command_buffer,
                                uint32_t image_index) {
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = this->render_pass;
  render_pass_info.framebuffer = this->swap_chain_framebuffers[image_index];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = this->swap_chain_extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clearColor;

  vkCmdBeginRenderPass(this->command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    this->graphics_pipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)this->swap_chain_extent.width;
  viewport.height = (float)this->swap_chain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(this->command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = this->swap_chain_extent;
  vkCmdSetScissor(this->command_buffer, 0, 1, &scissor);

  vkCmdDraw(this->command_buffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(this->command_buffer);

  if (vkEndCommandBuffer(this->command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void Lvk::create_sync_objects() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &this->image_available_semaphore) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &this->render_finished_semaphore) != VK_SUCCESS ||
      vkCreateFence(device, &fenceInfo, nullptr, &this->in_flight_fence) !=
          VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create synchronization objects for a frame!");
  }
}

VkShaderModule Lvk::create_shader_module(const std::vector<char> &code) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(this->device, &create_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shader_module;
}

void Lvk::run() {
  while (!glfwWindowShouldClose(this->window)) {
    glfwPollEvents();
    this->draw_frame();
  }

  vkDeviceWaitIdle(this->device);
}

void Lvk::draw_frame() {
  vkWaitForFences(this->device, 1, &this->in_flight_fence, VK_TRUE, UINT64_MAX);
  vkResetFences(this->device, 1, &this->in_flight_fence);

  uint32_t image_index;
  vkAcquireNextImageKHR(this->device, this->swap_chain, UINT64_MAX,
                        this->image_available_semaphore, VK_NULL_HANDLE,
                        &image_index);

  vkResetCommandBuffer(this->command_buffer,
                       /*VkCommandBufferResetFlagBits*/ 0);
  this->record_command_buffer(this->command_buffer, image_index);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {this->image_available_semaphore};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &this->command_buffer;

  VkSemaphore signal_semaphores[] = {this->render_finished_semaphore};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  if (vkQueueSubmit(this->graphics_queue, 1, &submit_info,
                    this->in_flight_fence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swap_chains[] = {this->swap_chain};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;

  present_info.pImageIndices = &image_index;

  vkQueuePresentKHR(this->present_queue, &present_info);
}

void Lvk::clean_up() {
  vkDestroySemaphore(this->device, this->render_finished_semaphore, nullptr);
  vkDestroySemaphore(this->device, this->image_available_semaphore, nullptr);
  vkDestroyFence(this->device, this->in_flight_fence, nullptr);

  // Command buffers are freed when the command pool is destroyed
  vkDestroyCommandPool(this->device, this->command_pool, nullptr);

  for (auto framebuffer : this->swap_chain_framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  vkDestroyPipeline(this->device, this->graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(this->device, this->pipeline_layout, nullptr);
  vkDestroyRenderPass(this->device, this->render_pass, nullptr);

  for (auto image_view : this->swap_chain_image_views) {
    vkDestroyImageView(this->device, image_view, nullptr);
  }

  vkDestroySwapchainKHR(this->device, this->swap_chain, nullptr);

  vkDestroyDevice(this->device, nullptr);

  vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

  if (enable_validation_layer) {
    utils::messenger::destroy_debug_utils_messenger_ext(
        this->instance, this->debug_messenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);

  // Finalize the windows
  glfwDestroyWindow(this->window);

  glfwTerminate();
}

Lvk::~Lvk() { this->clean_up(); }
} // namespace lvk
