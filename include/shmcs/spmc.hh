#ifndef SHMCS_SPMC_HH
#define SHMCS_SPMC_HH

#include <condition_variable>
#include <deque>
#include <mutex>

namespace shmcs {

/**
 * Single-Queue-Multiple-Consumers
 * ---
 * Thread-safe queue that allow multiple consumers
 * read from the single queue
 */
template<typename T>
class SPMCQueue {
  public:
  SPMCQueue() = default;

  auto produce(T val) -> void {
    std::lock_guard<std::mutex> lock_guard(mtx);
    queue.push_back(val);
    cond.notify_one();
  }

  auto consume() -> T {
    std::unique_lock<std::mutex> lock_guard(mtx);
    cond.wait(lock_guard, [this] { return !queue.empty(); });
    auto val = queue.front();
    queue.pop_front();
    return val;
  }

  private:
  std::deque<T> queue;
  std::mutex mtx{};
  std::condition_variable cond{};
};

} // namespace shmcs

#endif // SHMCS_SPMC_HH
