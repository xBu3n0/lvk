#ifndef LAYER_HPP
#define LAYER_HPP

#include <vector>

namespace utils {
namespace layer {
void get_layers();
std::vector<const char *> get_validation_layers();

bool check_validation_layers(
    const std::vector<const char *> &validation_layers);
} // namespace layer
} // namespace utils

#endif
