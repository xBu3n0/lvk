#ifndef _VLK_HPP
#define _VLK_HPP

#include <GLFW/glfw3.h>
#include "../Window/Window.hpp"

#include <map>
#include <memory>

namespace vlk {
    #define msw std::map<std::string, std::unique_ptr<window::Window>>

    class Vlk {
    // Controle geral da aplicação
    public:
        Vlk();

    private:
        void init();

    public:
        void run();
        void clean_up();
        ~Vlk();

    // Sistema de janelas
    public:
        void add_window(std::string name, uint32_t width, uint32_t height);
        void remove_window(const std::string& name);
    private:
        std::map<std::string, std::unique_ptr<window::Window>> windows;

    // Formas de acessar a Window
    public:
        void set_background_color(const std::string& name, float* background_color);
    };
}

#endif
