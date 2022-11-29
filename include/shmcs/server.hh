#ifndef SHMCS_SERVER_HH
#define SHMCS_SERVER_HH

#include "shmcs/handler.hh"

namespace shmcs {

class Server : Handler {
  public:
  auto handle_connection(Connection& con) -> void override;

  private:
  /// inserts new key to a bucket
  auto handle_insert(Connection& con, Message& msg) -> void;

  /// reads the content of a bucket
  auto handle_read(Connection& con, Message& msg) -> void;

  /// deletes a key from a bucket
  auto handle_delete(Connection& con, Message& msg) -> void;
};

} // namespace shmcs

#endif // SHMCS_SERVER_HH
