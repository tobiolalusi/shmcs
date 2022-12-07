#include <fcntl.h>

#include "shmcs/chained-hashtable.hh"
#include "shmcs/network/connection.hh"
#include "shmcs/network/server.hh"

namespace shmcs {

auto Server::run() -> std::thread {
  // worker threads
  for (unsigned i = 0; i < NUM_WORKERS; ++i) {
    workers[i] = std::thread(worker, std::ref(handler), std::ref(fd_queue));
  }

  // dispatcher thread
  auto thread = std::thread(dispatcher, std::ref(shm_path), std::ref(fd_queue));

  return thread;
}

/// typically a shared memory consumer;
/// listens to the shared memory via continuously reading from it
auto Server::dispatcher(shmcs::shm_path_t& shm_path, SPMCQueue<void*>& fd_queue)
    -> void {
  int fd = shm_open(shm_path, O_CREAT | O_TRUNC | O_RDONLY, SHM_PERMISSIONS);
  if (!fd) {
    throw std::runtime_error(fmt::format("Failed to open {}", shm_path));
  }
  if (!ftruncate(fd, SHM_SIZE)) {
    throw std::runtime_error("Failed to allocate shared memory");
  }
  auto memptr = mmap(nullptr, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
  if (memptr == MAP_FAILED) {
    throw std::runtime_error("Failed to map shared memory");
  }
  close(fd);
  while (true)
    ;
}

auto Server::worker(shmcs::ServerHandler& handler, SPMCQueue<void*>& fd_queue)
    -> void {
  while (true)
    ;
}

} // namespace shmcs