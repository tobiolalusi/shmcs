#ifndef SHMCS_UTILS_HH
#define SHMCS_UTILS_HH

#include <cstdlib>
#include <stdexcept>

namespace shmcs {
  auto toLong(char* str) -> long {
    char* endptr;
    long val = strtol(str, &endptr, 10);
    if (endptr == str) {
      throw std::runtime_error("Failed to convert string to integer");
    }
    return val;
  }
}

#endif // SHMCS_UTILS_HH
