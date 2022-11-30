#ifndef SHMCS_HASHTABLE_HH
#define SHMCS_HASHTABLE_HH

#include <functional>
#include <memory>

namespace shmcs {

/**
 * Key-only hash table with chaining
 * @tparam K
 */
template<typename K>
class HashTable {
  public:
  explicit HashTable(std::function<size_t(K)> hashfn, size_t size)
      : hashfn{hashfn}, size{size} {
    buckets = std::make_unique<K[]>(size);
  }

  /// inserts key into bucket based on the hash function
  auto insert(K k) -> void {
    auto index = hashfn(k) % size;
  }

  auto find(K k) -> bool;
  auto remove(K k) -> bool;

  private:
  size_t size;
  std::unique_ptr<K> buckets;
  std::function<void(K)> hashfn;
};

} // namespace shmcs

#endif // SHMCS_HASHTABLE_HH
