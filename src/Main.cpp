#include "Vlk/Vlk.hpp"

float colors[][4] = {
    {0.6f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.6f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.6f, 1.0f},
    {0.6f, 0.6f, 0.0f, 1.0f},
    {0.0f, 0.6f, 0.6f, 1.0f},
    {0.6f, 0.0f, 0.6f, 1.0f},
    {0.6f, 0.6f, 0.6f, 1.0f}
};

int main() {
    vlk::Vlk app;

    app.add_window("teste 1", 800, 600);
    app.add_window("teste 2", 800, 600);
    app.add_window("teste 3", 800, 600);
    app.add_window("teste 4", 800, 600);
    app.add_window("teste 5", 800, 600);
    app.add_window("teste 6", 800, 600);
    app.add_window("teste 7", 800, 600);

    app.set_background_color("teste 1", colors[0]);
    app.set_background_color("teste 2", colors[1]);
    app.set_background_color("teste 3", colors[2]);
    app.set_background_color("teste 4", colors[3]);
    app.set_background_color("teste 5", colors[4]);
    app.set_background_color("teste 6", colors[5]);
    app.set_background_color("teste 7", colors[6]);

    app.run();

    app.clean_up();

    return 0;
}
