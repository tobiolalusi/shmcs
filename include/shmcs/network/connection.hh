#ifndef SHMCS_NETWORK_CONNECTION_HH
#define SHMCS_NETWORK_CONNECTION_HH

#include "shm.pb.h"
#include "shmcs/types.hh"

#define SHM_SIZE 4096 // 4KiB
#define SHM_PERMISSIONS 0666 // -rw-rw-rw-

namespace shmcs {

const auto max_message_size = 4096;

/**
 * Shared Memory Buffer connection to server
 */
class Connection {
  public:
  explicit Connection(shm_path_t& shm_path);

  ~Connection();

  auto receive(Message& msg) const -> bool;

  auto send(const Message& msg) const -> bool;

  private:
  int fd{-1};
  void* memptr{nullptr};
};

} // namespace shmcs

#endif // SHMCS_NETWORK_CONNECTION_HH
