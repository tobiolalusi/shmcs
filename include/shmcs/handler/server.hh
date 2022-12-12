#ifndef SHMCS_HANDLER_SERVER_HH
#define SHMCS_HANDLER_SERVER_HH

#include "shmcs/chained-hashtable.hh"
#include "shmcs/handler/handler.hh"

namespace shmcs {

class ServerHandler : public Handler {
  public:
  explicit ServerHandler(size_t buckets);

  auto handle_connection(Connection& con) -> void override;

  private:
  /// inserts new key to a bucket
  auto handle_insert(Connection& con, Message& msg) -> void;

  /// reads the content of a bucket
  auto handle_read(Connection& con, Message& msg) -> void;

  /// deletes a key from a bucket
  auto handle_delete(Connection& con, Message& msg) -> void;

  ChainedHashTable<shm_key_t> hashtable;
};

} // namespace shmcs

#endif // SHMCS_HANDLER_SERVER_HH
