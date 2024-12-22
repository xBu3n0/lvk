#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <cstdint>
#include <string>

namespace window {
    class Window {
    public:
        Window(std::string title, uint32_t width, uint32_t height);
        ~Window();

        void set_background_color(float bg_color[4]);
        void update();
    private:
        float background_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    public:
        bool should_close();
    private:
        void set_current_context();


    private:
        GLFWwindow* window = nullptr;
        std::string title;
        uint32_t width, height;
    };
}

#endif
