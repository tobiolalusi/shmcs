#include "shmcs/network/server.hh"

using namespace shmcs;

// TODO: listen to termination signal and safely destruct server

auto main(int argc, char** argv) -> int {
  if (argc < 3) {
    fmt::print(
        "Usage: {} [PATH] [BUCKETS]\n"
        "Starts the shared memory server at [PATH] with [BUCKETS] buckets\n"
        "--\n"
        "Created by Oluwatobiloba Olalusi\n",
        argv[0]);
    return EXIT_FAILURE;
  }

  shm_name_t shm_path = argv[1];       // [PATH]
  size_t buckets = std::stol(argv[2]); // [BUCKETS]

  // server setup
  auto handler = ServerHandler{buckets};
  auto server = Server(shm_path, handler);
  auto server_thread = server.run();

  fmt::print("SHM server up and running...\n");

  server_thread.join();

  return EXIT_SUCCESS;
}
