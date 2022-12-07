#ifndef SHMCS_NETWORK_SERVER_H
#define SHMCS_NETWORK_SERVER_H

#include <sys/mman.h>
#include <thread>
#include <utility>

#include "shmcs/handler/server.hh"
#include "shmcs/spmc.hh"

namespace shmcs {

class Server {
  public:
  static const auto NUM_WORKERS = 4;

  Server(shm_path_t shm_path, ServerHandler& handler)
      : shm_path{shm_path}, handler{handler} {}

  /// unique server
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&&) = delete;

  ~Server() {
    shm_unlink(shm_path); // TODO: confirm this behaviour
    for (auto& worker : workers) { worker.join(); }
  }

  /// spawn dispatcher and worker threads
  auto run() -> std::thread;

  private:
  /// dispatcher thread routine for listening for incoming connections
  /// and dispatching requests to worker threads
  static auto dispatcher(shm_path_t& shm_path, SPMCQueue<void*>& fd_queue)
      -> void;

  /// worker thread routine for handling requests
  static auto worker(ServerHandler& handler, SPMCQueue<void*>& fd_queue)
      -> void;

  shm_path_t shm_path;
  std::array<std::thread, NUM_WORKERS> workers;
  SPMCQueue<void*> fd_queue;
  ServerHandler& handler;
};

} // namespace shmcs

#endif //SHMCS_NETWORK_SERVER_H
