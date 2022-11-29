#include "shmcs/network/server.hh"

namespace shmcs {

auto Server::run() -> std::thread {
  // worker threads
  for (unsigned i = 0; i < num_workers; ++i) {
    workers[i] = std::thread(worker, std::ref(handler), std::ref(fd_queue));
  }

  // dispatcher thread
  auto thread = std::thread(dispatcher, std::ref(shm_name), std::ref(fd_queue));

  return thread;
}

auto Server::dispatcher(shmcs::shm_name_t& shm_name, SPMCQueue<void*>& fd_queue)
    -> void {
  while (true);
}

auto Server::worker(shmcs::ServerHandler& handler, SPMCQueue<void*>& fd_queue)
    -> void {
  while (true);
}

} // namespace shmcs