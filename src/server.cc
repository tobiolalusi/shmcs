#include "shmcs/network/server.hh"
#include <csignal>

using namespace shmcs;

shm_name_t shm_name;

auto sigTermHandler(int) -> void {
  if (!std::string(shm_name).empty()) {
    std::cout << "Safely terminating server " << shm_name << "...\n";
    shm_unlink(shm_name);
    auto shm_sem_r_name = std::string(shm_name).append("-r").c_str();
    sem_unlink(shm_sem_r_name);
    auto shm_sem_w_name = std::string(shm_name).append("-w").c_str();
    sem_unlink(shm_sem_w_name);
    auto shm_sem_s_name = std::string(shm_name).append("-s").c_str();
    sem_unlink(shm_sem_s_name);
  }
  exit(EXIT_SUCCESS);
}

auto main(int argc, char** argv) -> int {
  std::signal(SIGILL, &sigTermHandler);
  std::signal(SIGABRT, &sigTermHandler);
  std::signal(SIGFPE, &sigTermHandler);
  std::signal(SIGSEGV, &sigTermHandler);
  std::signal(SIGTERM, &sigTermHandler);
  std::signal(SIGQUIT, &sigTermHandler);
  std::signal(SIGKILL, &sigTermHandler);

  if (argc < 3) {
    fmt::print(
        "Usage: {} [PATH] [BUCKETS]\n"
        "Starts the shared memory server at [PATH] with [BUCKETS] buckets\n"
        "--\n"
        "Created by Oluwatobiloba Olalusi\n",
        argv[0]);
    return EXIT_FAILURE;
  }

  shm_name = argv[1];                  // [PATH]
  size_t buckets = std::stol(argv[2]); // [BUCKETS]

  // server setup
  auto handler = ServerHandler{buckets};
  auto server = Server(shm_name, handler);
  auto server_thread = server.run();

  fmt::print("SHM server up and running...\n");

  server_thread.join();

  return EXIT_SUCCESS;
}
