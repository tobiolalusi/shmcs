#include <fcntl.h>

#include "shmcs/chained-hashtable.hh"
#include "shmcs/network/connection.hh"
#include "shmcs/network/server.hh"

namespace shmcs {

auto Server::run() -> std::thread {
  // setup shared memory
  int fd = shm_open(name, O_CREAT | O_TRUNC | O_RDWR, SHM_PERMISSIONS);
  if (fd == -1) {
    throw std::runtime_error(
        fmt::format("Failed to open \"{}\": {}", name, std::strerror(errno)));
  }
  if (ftruncate(fd, SHM_SIZE) == -1) {
    throw std::runtime_error(fmt::format("Failed to allocate shared memory: {}",
                                         std::strerror(errno)));
  }
  auto memptr =
      mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (memptr == MAP_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to map shared memory: {}", std::strerror(errno)));
  }
  close(fd);

  // dispatcher thread
  auto thread = std::thread(dispatcher, std::ref(name), std::ref(handler));

  return thread;
}

auto Server::dispatcher(shmcs::shm_name_t& name, ServerHandler& handler)
    -> void {
  // shared memory reading semaphore
  auto shm_sem_r_name = std::string(name).append("-r").c_str();
  auto shm_sem_r =
      sem_open(shm_sem_r_name, O_CREAT | O_EXCL, SHM_PERMISSIONS, 0);
  if (shm_sem_r == SEM_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to open shared memory reading semaphore: {}",
                    std::strerror(errno)));
  }

  // shared memory writing semaphore
  auto shm_sem_w_name = std::string(name).append("-w").c_str();
  auto shm_sem_w =
      sem_open(shm_sem_w_name, O_CREAT | O_EXCL, SHM_PERMISSIONS, 1);
  if (shm_sem_w == SEM_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to open shared memory writing semaphore: {}",
                    std::strerror(errno)));
  }

  // shared memory server operation semaphore
  auto shm_sem_s_name = std::string(name).append("-s").c_str();
  auto shm_sem_s =
      sem_open(shm_sem_s_name, O_CREAT | O_EXCL, SHM_PERMISSIONS, 0);
  if (shm_sem_s == SEM_FAILED) {
    throw std::runtime_error(fmt::format(
        "Failed to open shared memory server operation semaphore: {}",
        std::strerror(errno)));
  }

  // setup connection to handle requests
  Connection conn{name, true};

  // listen to the shared memory by continuously reading from it
  while (true) {
    // server waits until it is allowed to handle request
    sem_wait(shm_sem_s);

    // do the real thing
    handler.handle_connection(conn);

    // clients are now allowed to read
    sem_post(shm_sem_r);
  }
}

} // namespace shmcs
