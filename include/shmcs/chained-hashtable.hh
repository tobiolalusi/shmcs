#ifndef SHMCS_CHAINED_HASHTABLE_HH
#define SHMCS_CHAINED_HASHTABLE_HH

#include <fmt/core.h>
#include <forward_list>
#include <functional>
#include <memory>

namespace shmcs {

struct bucket_not_found : std::runtime_error {
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
  explicit ChainedHashTable(const std::function<size_t(K)> hashfn, size_t size)
      : hashfn{hashfn}, size{size},
        buckets{std::make_unique<std::forward_list<K>[]>(size)} {}

  /// inserts key into bucket based on the hash function
  auto insert(K k) -> void {
    std::forward_list<K>* bucket = buckets.get() + bucket_idx(k);
    bucket->push_front(k);
  }

  /// reads the content of a bucket
  auto read(size_t b) -> std::forward_list<K> {
    if (size < b) {
      throw bucket_not_found{fmt::format("Bucket {} not found!", b)};
    }
    std::forward_list<K>* bucket = buckets.get() + b;
    return *bucket;
  }

  /// removes item from bucket
  auto remove(K k) -> size_t {
    std::forward_list<K>* bucket = buckets.get() + bucket_idx(k);
    return bucket->remove(k);
  }

  private:
  /// calculates the index of the bucket key belongs to
  auto bucket_idx(K k) -> size_t { return hashfn(k) % size; }

  size_t size;
  std::unique_ptr<std::forward_list<K>[]> buckets;
  std::function<size_t(K)> hashfn;
};

} // namespace shmcs

#endif // SHMCS_CHAINED_HASHTABLE_HH
