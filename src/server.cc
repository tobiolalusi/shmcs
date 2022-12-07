#include "shmcs/network/server.hh"

using namespace shmcs;

auto main(int argc, char** argv) -> int {
  if (argc < 3) {
    fmt::print("Usage: {} [PATH] [BUCKETS]\n"
               "Starts the shared memory (shm) server at [PATH] with [BUCKETS] "
               "number of buckets\n"
               "--\n"
               "Created by Oluwatobiloba Olalusi\n",
               argv[0]);
    return EXIT_FAILURE;
  }

  // [PATH] assignment and handling
  shm_path_t shm_path = argv[1];
  if (shm_path[0] != '/') {
    throw std::runtime_error("[PATH] should start with a forward slash \"/\"");
  }
  if (strlen(shm_path) > NAME_MAX) {
    throw std::runtime_error("[PATH] cannot have more than 255 characters");
  }

  // [BUCKETS] assignment
  char* endptr;
  size_t buckets = strtol(argv[2], &endptr, 10);
  if (endptr == argv[2]) return EXIT_FAILURE;

  // server setup
  auto handler = ServerHandler{buckets};
  auto server = Server(shm_path, handler);
  auto server_thread = server.run();

  fmt::print("SHM server up and running...\n");

  server_thread.join();

  return EXIT_SUCCESS;
}
