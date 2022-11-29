#include "shmcs/network/server.hh"

using namespace shmcs;

auto main(int argc, char** argv) -> int {
  size_t buckets;
  shm_name_t shm_name;

  if (argc < 3) {
    fmt::print("Usage: {} [NAME] [BUCKETS]\n"
               "Starts the shared memory (shm) server at [NAME] with [BUCKETS] "
               "number of buckets\n"
               "--\nCreated by Oluwatobiloba Olalusi\n",
               argv[0]);
    return EXIT_FAILURE;
  }

  char* endptr;
  buckets = strtol(argv[2], &endptr, 10);
  if (endptr == argv[2]) return EXIT_FAILURE;
  shm_name = argv[1];

  auto handler = ServerHandler{buckets};
  auto server = Server(shm_name, handler);
  auto server_thread = server.run();

  fmt::print("SHM server up and running...\n");

  server_thread.join();

  return EXIT_SUCCESS;
}
