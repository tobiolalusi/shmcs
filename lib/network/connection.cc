#include <netdb.h>
#include <sys/mman.h>

#include "shmcs/network/connection.hh"

namespace shmcs {

Connection::Connection(shm_name_t& name, bool is_server)
    : is_server{is_server} {
  // connect to shared memory created by server
  int fd = shm_open(name, O_RDWR, SHM_PERMISSIONS);
  if (fd == -1) {
    throw std::runtime_error(
        fmt::format("Failed to open \"{}\": {}", name, std::strerror(errno)));
  }
  memptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (memptr == MAP_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to map shared memory: {}", std::strerror(errno)));
  }
  close(fd);

  // shared memory reading semaphore
  auto shm_sem_r_name = std::string(name).append("-r").c_str();
  shm_sem_r = sem_open(shm_sem_r_name, O_RDWR);
  if (shm_sem_r == SEM_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to open shared memory reading semaphore: {}",
                    std::strerror(errno)));
  }

  // shared memory writing semaphore
  auto shm_sem_w_name = std::string(name).append("-w").c_str();
  shm_sem_w = sem_open(shm_sem_w_name, O_RDWR);
  if (shm_sem_w == SEM_FAILED) {
    throw std::runtime_error(
        fmt::format("Failed to open shared memory writing semaphore: {}",
                    std::strerror(errno)));
  }

  // shared memory server operation semaphore
  auto shm_sem_s_name = std::string(name).append("-s").c_str();
  shm_sem_s = sem_open(shm_sem_s_name, O_RDWR);
  if (shm_sem_s == SEM_FAILED) {
    throw std::runtime_error(fmt::format(
        "Failed to open shared memory server operation semaphore: {}",
        std::strerror(errno)));
  }
}

Connection::~Connection() {
  sem_close(shm_sem_r);
  sem_close(shm_sem_w);
  sem_close(shm_sem_s);
  munmap(memptr, SHM_SIZE);
}

auto Connection::receive(shmcs::Message& msg) const -> bool {
  // clients wait until they are allowed to read from shared memory
  if (!is_server) sem_wait(shm_sem_r);

  // extract message size (first 4 bytes)
  size_t size{};
  memcpy(&size, memptr, 4);
  if (size == 0) {
    if (!is_server) sem_post(shm_sem_w);
    throw std::runtime_error("[ERROR]: Received empty message");
  }
  if (size > SHM_SIZE) {
    if (!is_server) sem_post(shm_sem_w);
    throw std::runtime_error(
        "[ERROR]: Received message exceeding the maximum message size");
  }

  // deserialize message from shared memory
  msg.ParseFromArray(static_cast<char*>(memptr) + 4, static_cast<int>(size));
  std::memset(memptr, 0, SHM_SIZE); // empty shared memory

  // clients are now allowed to send new messages
  if (!is_server) sem_post(shm_sem_w);

  return true;
}

auto Connection::send(const shmcs::Message& msg) const -> bool {
  // clients wait until they are allowed to write to shared memory
  if (!is_server) sem_wait(shm_sem_w);

  // set first 4 bytes to size of message
  size_t size = msg.ByteSizeLong();
  memcpy(memptr, &size, 4);

  // write serialized message to shared memory
  msg.SerializeToArray(static_cast<char*>(memptr) + 4, static_cast<int>(size));

  // server can now handle the request
  if (!is_server) sem_post(shm_sem_s);

  return true;
}

} // namespace shmcs
