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

  ~Server() {
    shm_unlink(name);
    // TODO: create utility function for this
    auto shm_sem_r_name = std::string(name).append("-r").c_str();
    sem_unlink(shm_sem_r_name);
    auto shm_sem_w_name = std::string(name).append("-w").c_str();
    sem_unlink(shm_sem_w_name);
    auto shm_sem_s_name = std::string(name).append("-s").c_str();
    sem_unlink(shm_sem_s_name);
  }

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
