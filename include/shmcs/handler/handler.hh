#ifndef SHMCS_HANDLER_HANDLER_HH
#define SHMCS_HANDLER_HANDLER_HH

#include "shmcs/network/connection.hh"

namespace shmcs {

class Handler {
  public:
  virtual ~Handler() = default;

  virtual auto handle_connection(Connection& con) -> void = 0;
};

} // namespace shmcs

#endif // SHMCS_HANDLER_HANDLER_HH
