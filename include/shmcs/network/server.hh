#ifndef SHMCS_NETWORK_SERVER_H
#define SHMCS_NETWORK_SERVER_H

#include <thread>
#include <utility>

#include "shmcs/handler/server.hh"
#include "shmcs/spmc.hh"

namespace shmcs {

const auto num_workers = 4;

class Server {
  public:
  Server(shm_name_t shm_name, ServerHandler& handler)
      : shm_name{std::move(shm_name)}, handler{handler} {}

  /// unique server
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&&) = delete;

  ~Server() {
    for (auto& worker : workers) { worker.join(); }
  }

  /// spawn dispatcher and worker threads
  auto run() -> std::thread;

  private:
  /// dispatcher thread routine for listening for incoming connections
  /// and dispatching requests to worker threads
  static auto dispatcher(shm_name_t& shm_name, SPMCQueue<void*>& fd_queue)
      -> void;

  /// worker thread routine for handling requests
  static auto worker(ServerHandler& handler, SPMCQueue<void*>& fd_queue)
      -> void;

  shm_name_t shm_name;
  std::array<std::thread, num_workers> workers;
  SPMCQueue<void*> fd_queue;
  ServerHandler& handler;
};

} // namespace shmcs

#endif //SHMCS_NETWORK_SERVER_H
