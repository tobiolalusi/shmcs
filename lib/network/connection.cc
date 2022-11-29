#include <netdb.h>

#include "shmcs/network/connection.hh"

namespace shmcs {

auto Connection::receive(shmcs::Message& msg) const -> bool {
  // read message size
  uint32_t size{}, read_bytes{};
  read_bytes = read(fd, &size, 4);
  if (read_bytes < 4) {
    if (read_bytes == 0) return false; // connection closed
    throw std::runtime_error(fmt::format(
        "[ERROR]: Received incomplete message of {} bytes\n", read_bytes));
  }

  // size to host byte order
  size = ntohl(size);

  if (size > max_message_size) {
    throw std::runtime_error(
        "[ERROR]: Received message exceeding the maximum message size\n");
  }

  auto buffer = std::make_unique<uint8_t[]>(size);

  read_bytes = read(fd, buffer.get(), size);

  if (read_bytes < size) {
    throw std::runtime_error(fmt::format(
        "[ERROR]: Received incomplete message of {} bytes; expected {} bytes\n",
        read_bytes, size));
  }

  msg.ParseFromArray(buffer.get(), size);

  return read_bytes == size;
}

auto Connection::send(shmcs::Message& msg) const -> bool {
  uint32_t size = msg.ByteSizeLong();
  auto buffer = std::make_unique<uint8_t[]>(size + 4);

  // set first 4 bytes to size of message in network byte order
  uint32_t size_nl = htonl(size);
  memcpy(buffer.get(), &size_nl, 4);

  // serialize message
  msg.SerializeToArray(buffer.get() + 4, size);
  return write(fd, buffer.get(), size + 4) == size + 4;
}

} // namespace shmcs
