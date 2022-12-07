#include "shmcs/network/connection.hh"
#include "shmcs/utils.hh"

using namespace shmcs;

auto main(int argc, char** argv) -> int {
  if (argc < 3) {
    fmt::print("Usage: {} [PATH] [OPERATION] [KEY|BUCKET]\n"
               "Client to send operations to execute on the server\n"
               "[OPERATION] with [KEY|BUCKET] may be combined as follows:\n"
               "\tinsert [KEY]: inserts key into the hashtable\n"
               "\tdelete [KEY]: removes key from the hashtable\n"
               "\tread [BUCKET]: reads all the keys in the bucket\n"
               "--\n"
               "Created by Oluwatobiloba Olalusi\n",
               argv[0], argv[0]);
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

  // [OPERATION] assignment
  Message::Operation operation;
  if (strcmp(argv[2], "insert") == 0) {
    operation = shmcs::Message_Operation_INSERT;
  } else if (strcmp(argv[2], "delete") == 0) {
    operation = shmcs::Message_Operation_DELETE;
  } else if (strcmp(argv[2], "read") == 0) {
    operation = shmcs::Message_Operation_READ;
  } else {
    throw std::runtime_error("Unknown [OPERATION]");
  }

  // setup requests to server
  Message request{};
  request.set_type(shmcs::Message_Type_REQUEST);
  request.set_operation(operation);

  switch (operation) {
    case shmcs::Message_Operation_INSERT:
    case shmcs::Message_Operation_DELETE:
      request.add_key(argv[3]);
      break;
    case shmcs::Message_Operation_READ: {
      shm_bucket_t bucket = shmcs::toLong(argv[3]);
      request.set_bucket(bucket);
      break;
    }
  }

  // establish connection
  Connection conn{shm_path};

  // send request
  conn.send(request);

  // receive response
  Message response{};
  conn.receive(response);

  if (!response.success()) {
    fmt::print("OPERATION FAILED:\n");
  }
  fmt::print("{}\n", response.message());

  return EXIT_SUCCESS;
}
