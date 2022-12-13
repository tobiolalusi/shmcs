#ifndef SHMCS_NETWORK_SERVER_H
#define SHMCS_NETWORK_SERVER_H

#include <sys/mman.h>
#include <thread>
#include <utility>

#include "shmcs/handler/server.hh"

namespace shmcs {

class Server {
  public:
  Server(shm_name_t name, ServerHandler& handler)
      : name{name}, handler{handler} {}

  /// unique server
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&&) = delete;

  /// spawn dispatcher and worker threads
  auto run() -> std::thread;

  private:
  /// dispatcher thread routine for listening for incoming connections
  [[noreturn]] static auto dispatcher(shm_name_t& name, ServerHandler& handler)
      -> void;

  shm_name_t name;
  ServerHandler& handler;
};

} // namespace shmcs

#endif //SHMCS_NETWORK_SERVER_H
