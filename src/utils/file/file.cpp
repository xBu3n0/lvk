#include "file.hpp"
#include <fstream>

namespace utils {
namespace file {
std::vector<char> read_file(const std::string &path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;
}
} // namespace file
} // namespace utils
