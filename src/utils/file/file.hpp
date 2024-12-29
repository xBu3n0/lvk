#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <vector>

namespace utils {
namespace file {
std::vector<char> read_file(const std::string &path);
} // namespace file
} // namespace utils

#endif
