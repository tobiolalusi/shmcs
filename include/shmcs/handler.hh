#ifndef SHMCS_HANDLER_HH
#define SHMCS_HANDLER_HH

#include "shmcs/connection.hh"

namespace shmcs {

class Handler {
  public:
  virtual ~Handler() = default;

  virtual auto handle_connection(Connection& con) -> void;
};

} // namespace shmcs

#endif // SHMCS_HANDLER_HH
