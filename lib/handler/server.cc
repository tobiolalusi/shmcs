#include "shmcs/handler/server.hh"

namespace shmcs {

auto hashfn(shm_key_t key) -> shm_bucket_t {
  return std::hash<std::string>{}(key);
}

ServerHandler::ServerHandler(size_t buckets) : hashtable{hashfn, buckets} {}

auto ServerHandler::handle_connection(shmcs::Connection& con) -> void {
  Message request{}, response{};

  if (!con.receive(request)) return;

  if (request.type() != Message_Type_REQUEST) {
    throw std::runtime_error("[ERROR]: Expected a request\n");
  }

  switch (request.operation()) {
    case Message_Operation_INSERT:
      handle_insert(con, request);
      break;
    case Message_Operation_READ:
      handle_read(con, request);
      break;
    case Message_Operation_DELETE:
      handle_delete(con, request);
      break;
    default:
      response.set_operation(request.operation());
      response.set_success(false);
      auto message = "Operation not supported!";
      response.set_message(message);
#ifdef INFO
      fmt::print("[INFO]: {}\n", message);
#endif
      con.send(response);
      break;
  }
}

auto ServerHandler::handle_insert(shmcs::Connection& con, shmcs::Message& msg)
    -> void {
#ifdef DEBUG
  fmt::print("[DEBUG]: Received INSERT request\n");
#endif
  Message response{};
  response.set_type(Message_Type_RESPONSE);
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  for (const auto& kbp : msg.kbp()) {
    auto b = hashtable.insert(kbp.key());
#ifdef INFO
    fmt::print("Inserted key \"{}\" into bucket {}\n", kbp.key(), b);
#endif
    auto* tmp = response.add_kbp();
    tmp->set_key(kbp.key());
    tmp->set_bucket(b);
  }

  con.send(response);
}

auto ServerHandler::handle_read(shmcs::Connection& con, shmcs::Message& msg)
    -> void {
  auto bucket = msg.kbp()[0].bucket();
#ifdef DEBUG
  fmt::print("[DEBUG]: Received READ request for bucket {}\n", bucket);
#endif
  Message response{};
  response.set_type(Message_Type_RESPONSE);
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  try {
    auto keys = hashtable.read(bucket);
    for (const auto& key : keys) {
      auto* tmp = response.add_kbp();
      tmp->set_key(key);
      tmp->set_bucket(bucket);
    }
  } catch (bucket_not_found& e) {
    response.set_success(false);
    response.set_message(e.what());
  }

  con.send(response);
}

auto ServerHandler::handle_delete(shmcs::Connection& con, shmcs::Message& msg)
    -> void {
#ifdef DEBUG
  fmt::print("[DEBUG]: Received DELETE request\n");
#endif
  Message response{};
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  for (const auto& kbp : msg.kbp()) {
    auto* tmp = response.add_kbp();
    tmp->set_key(kbp.key());
    try {
      auto b = hashtable.remove(kbp.key());
      tmp->set_bucket(b);
#ifdef INFO
      fmt::print("Deleted key \"{}\" from bucket {}\n", kbp.key(), b);
#endif
    } catch (key_not_found& e) {
      tmp->set_bucket(-1);
#ifdef INFO
      fmt::print("{}\n", e.what());
    }
#endif
  }

  con.send(response);
}

} // namespace shmcs
