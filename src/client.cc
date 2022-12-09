#include "shmcs/network/connection.hh"

using namespace shmcs;

auto main(int argc, char** argv) -> int {
  if (argc < 4) {
    fmt::print("Usage: {} [SHM_NAME] [OPERATION] [ARGS...]\n"
               "Client to send operations to execute on the server\n"
               "[SHM_NAME] represents the name of the shared memory of the\n"
               "\tserver the client is connecting to\n"
               "[OPERATION] with [ARGS...] may be combined as follows:\n"
               "\tinsert [KEY...]: insert keys into the hashtable\n"
               "\tdelete [KEY...]: remove keys from the hashtable\n"
               "\tread [BUCKET]: reads all the keys in the bucket\n"
               "--\n"
               "Created by Oluwatobiloba Olalusi\n",
               argv[0], argv[0]);
    return EXIT_FAILURE;
  }

  shm_name_t send_path = argv[1];      // [SEND_PATH]
  std::string operation = argv[2];     // [OPERATION]
  std::string operation_arg = argv[3]; // [ARGS...]

  // setup requests to server
  Message request{}, response{};
  request.set_type(Message_Type_REQUEST);
  if (operation == "insert") {
    request.set_operation(Message_Operation_INSERT);
    for (int i = 3; i < argc; ++i) {
      auto* tmp = request.add_kbp();
      tmp->set_key(argv[i]);
    }
  } else if (operation == "delete") {
    request.set_operation(Message_Operation_DELETE);
    for (int i = 3; i < argc; ++i) {
      auto* tmp = request.add_kbp();
      tmp->set_key(argv[i]);
    }
  } else if (operation == "read") {
    request.set_operation(Message_Operation_READ);
    shm_bucket_t bucket = std::stol(operation_arg);
    auto* tmp = request.add_kbp();
    tmp->set_bucket(bucket);
  } else {
    throw std::runtime_error("Unknown [OPERATION]");
  }

  // establish connection to server for sending
  Connection conn{send_path};
  conn.send(request);
  conn.receive(response);

  if (!response.success()) {
    fmt::print("OPERATION FAILED: {}\n", response.message());
  }

  if (response.operation() == Message_Operation_INSERT) {
    fmt::print("Inserted keys into hashtable:\n");
    for (const auto& kbp : response.kbp()) {
      fmt::print("\t[{}] {}\n", kbp.bucket(), kbp.key());
    }
  } else if (response.operation() == Message_Operation_READ) {
    fmt::print("Read keys from bucket {}:\n", argv[3]);
    for (const auto& kbp : response.kbp()) {
      fmt::print("\t[{}] {}\n", kbp.bucket(), kbp.key());
    }
  } else if (response.operation() == Message_Operation_DELETE) {
    fmt::print("Deleted keys from hashtable:\n");
    for (const auto& kbp : response.kbp()) {
      if (kbp.bucket() == static_cast<uint32_t>(-1)) {
        fmt::print("\t[*] {} [NON-EXISTENT]\n", kbp.key());
      } else {
        fmt::print("\t[{}] {} [DELETED]\n", kbp.bucket(), kbp.key());
      }
    }
  }

  return EXIT_SUCCESS;
}
