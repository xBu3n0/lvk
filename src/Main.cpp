#include "Lvk/Lvk.hpp"
#include <cstdlib>
#include <iostream>

float colors[][4] = {{0.6f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.6f, 0.0f, 1.0f},
                     {0.0f, 0.0f, 0.6f, 1.0f}, {0.6f, 0.6f, 0.0f, 1.0f},
                     {0.0f, 0.6f, 0.6f, 1.0f}, {0.6f, 0.0f, 0.6f, 1.0f},
                     {0.6f, 0.6f, 0.6f, 1.0f}};

int main() {
  lvk::Lvk app;

  // std::cout << "Adding windows" << std::endl;
  // app.add_window("teste 1", 800, 600);
  // app.add_window("teste 2", 800, 600);

  // std::cout << "Setting background colors" << std::endl;
  // app.set_background_color("teste 1", colors[0]);
  // app.set_background_color("teste 2", colors[1]);

  std::cout << "Running" << std::endl;
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;

    return EXIT_FAILURE;
  }

  std::cout << "Exiting" << std::endl;
  return EXIT_SUCCESS;
}
