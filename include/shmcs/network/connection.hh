#ifndef SHMCS_NETWORK_CONNECTION_HH
#define SHMCS_NETWORK_CONNECTION_HH

#include <fcntl.h>
#include <semaphore>

#include "shm.pb.h"
#include "shmcs/types.hh"

#define SHM_SIZE 4096        // 4KiB
#define SHM_PERMISSIONS 0666 // -rw-rw-rw-

namespace shmcs {

/**
 * Shared Memory Buffer connection to server abstraction
 */
class Connection {
  public:
  explicit Connection(shm_name_t& name, bool is_server = false);

  ~Connection();

  auto receive(Message& msg) const -> bool;

  auto send(const Message& msg) const -> bool;

  private:
  bool is_server;        // who is setting up the connection (server/client)?
  sem_t* shm_sem_r;      // shared memory semaphore for reading
  sem_t* shm_sem_w;      // shared memory semaphore for writing
  sem_t* shm_sem_s;      // shared memory semaphore for server operations
  void* memptr{nullptr}; // pointer to mapped memory
};

} // namespace shmcs

#endif // SHMCS_NETWORK_CONNECTION_HH
