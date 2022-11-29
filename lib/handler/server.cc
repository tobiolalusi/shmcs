#include "shmcs/handler/server.hh"

namespace shmcs {

auto ServerHandler::handle_connection(shmcs::Connection& con) -> void {
  Message request{}, response{};

  if (!con.receive(request)) return;

  if (request.type() != Message_Type_REQUEST) {
    throw std::runtime_error("[ERROR]: Expected a request\n");
  }

  switch (request.operation()) {
    case Message_Operation_INSERT:
#ifdef DEBUG
    fmt::print("[DEBUG]: Received INSERT request\n");
#endif
      handle_insert(con, request);
      break;
    case Message_Operation_READ:
#ifdef DEBUG
      fmt::print("[DEBUG]: Received READ request\n");
#endif
      handle_read(con, request);
      break;
    case Message_Operation_DELETE:
#ifdef DEBUG
      fmt::print("[DEBUG]: Received DELETE request\n");
#endif
      handle_delete(con, request);
      break;
    default:
      response.set_type(Message_Type_RESPONSE);
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

auto ServerHandler::handle_insert(shmcs::Connection& con, shmcs::Message& msg) -> void {
  Message response{};
  response.set_type(Message_Type_RESPONSE);
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  // TODO: perform insert here

  con.send(response);
}

auto ServerHandler::handle_read(shmcs::Connection& con, shmcs::Message& msg) -> void {
  Message response{};
  response.set_type(Message_Type_RESPONSE);
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  // TODO: perform read here

  con.send(response);
}

auto ServerHandler::handle_delete(shmcs::Connection& con, shmcs::Message& msg) -> void {
  Message response{};
  response.set_type(Message_Type_RESPONSE);
  response.set_operation(msg.operation());
  response.set_success(true);
  response.set_message("OK");

  // TODO: perform delete here

  con.send(response);
}

}
