#ifndef SHMCS_CHAINED_HASHTABLE_HH
#define SHMCS_CHAINED_HASHTABLE_HH

#include <fmt/core.h>
#include <forward_list>
#include <functional>
#include <memory>

#include "shmcs/types.hh"

namespace shmcs {

class bucket_not_found : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class key_not_found : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * Key-only hash table with chaining
 * @tparam K
 */
template<typename K>
class ChainedHashTable {
  public:
  /// creates a chained hash table of a specified `size`, distributed
  /// with a hash function `hashfn`
  explicit ChainedHashTable(const std::function<shm_bucket_t(K)> hashfn,
                            size_t size)
      : hashfn{hashfn}, size{size},
        buckets{std::make_unique<std::forward_list<K>[]>(size)} {}

  /// inserts key into bucket based on the hash function
  /// and returns the index of the bucket it was inserted into
  auto insert(K k) -> size_t {
    auto b = bucket_idx(k);
    std::forward_list<K>* bucket = buckets.get() + b;
    bucket->push_front(k);
    return b;
  }

  /// reads the content of a bucket
  /// and returns a list of the content of the bucket
  auto read(size_t b) -> std::forward_list<K> {
    if (size < b) {
      throw bucket_not_found{fmt::format("Bucket {} not found!", b)};
    }
    std::forward_list<K>* bucket = buckets.get() + b;
    return *bucket;
  }

  /// removes item from bucket
  /// and returns the bucket from which it was removed from
  auto remove(K k) -> size_t {
    auto b = bucket_idx(k);
    std::forward_list<K>* bucket = buckets.get() + b;
    if (bucket->remove(k) > 0) return b;
    throw key_not_found{fmt::format("Key \"{}\" not found!", k)};
  }

  private:
  /// calculates the index of the bucket key belongs to
  auto bucket_idx(K k) -> size_t { return hashfn(k) % size; }

  std::function<size_t(K)> hashfn;
  size_t size;
  std::unique_ptr<std::forward_list<K>[]> buckets;
};

} // namespace shmcs

#endif // SHMCS_CHAINED_HASHTABLE_HH
