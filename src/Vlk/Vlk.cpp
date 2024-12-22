#include "Vlk.hpp"
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <memory>
#include <stack>

namespace vlk {
    Vlk::Vlk() {
        Vlk::init();
    }

    void Vlk::init() {
        if (!glfwInit()) {
            exit(1);
        }
    }

    void Vlk::add_window(std::string name, uint32_t width, uint32_t height) {
        this->windows.insert({
            name,
            std::make_unique<window::Window>(name, width, height)
        });
    }

    void Vlk::remove_window(const std::string& name) {
        this->windows.erase(name);
    }

    void Vlk::set_background_color(const std::string& name, float background_color[4]) {
        this->windows[name]->set_background_color(background_color);
    }

    void Vlk::run() {
        std::stack<std::string> to_remove;

        while (!this->windows.empty()) {
            for (const auto& window : this->windows) {
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

    void Vlk::clean_up() {
        glfwTerminate();
    }

    Vlk::~Vlk() {

    }
}
