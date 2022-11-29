#ifndef SHMCS_TYPES_HH
#define SHMCS_TYPES_HH

#include <fmt/core.h>
#include <string>

namespace shmcs {

/// represents the name of the shared memory buffer
using shm_name_t = std::string;

/// represents a key that would be operated on
using shm_key_t = uint32_t;

} // namespace shmcs

#endif // SHMCS_TYPES_HH
