#ifndef SHMCS_NETWORK_CONNECTION_HH
#define SHMCS_NETWORK_CONNECTION_HH

#include "shm.pb.h"
#include "shmcs/types.hh"

namespace shmcs {

const auto max_message_size = 4096;

/**
 * Shared Memory Buffer connection abstraction
 */
class Connection {
  public:
  explicit Connection(int fd) : fd{fd} {};

  ~Connection() {
    close(fd);
  }

  auto receive(Message& msg) const -> bool;

  auto send(Message& msg) const -> bool;

  private:
  int fd{-1};
};

} // namespace shmcs

#endif // SHMCS_NETWORK_CONNECTION_HH