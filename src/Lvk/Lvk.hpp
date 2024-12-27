#ifndef _VLK_HPP
#define _VLK_HPP

// Load the Vulkan header
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Window/Window.hpp"

#include <map>
#include <memory>

namespace lvk {
#define msw std::map<std::string, std::unique_ptr<window::Window>>

class Lvk {
    // Controle geral da aplicação
public:
    Lvk();

private:
    void init();

    /**
     * Vulkan's niche is to be explicit about every operation to avoid mistakes
     */
    void init_vulkan();

    // The instance is the connection between your application and the Vulkan
    // library and creating it involves specifying some details about your
    // application to the driver.
    void create_instance();
    void create_debug_messenger();

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;

public:
    void run();
    ~Lvk();

private:
    void clean_up();

    // Sistema de janelas
public:
    void add_window(std::string name, uint32_t width, uint32_t height);
    void remove_window(const std::string &name);

private:
    std::map<std::string, std::unique_ptr<window::Window>> windows;

    // Formas de acessar a Window
public:
    void set_background_color(const std::string &name, float *background_color);
};
} // namespace lvk

#endif
