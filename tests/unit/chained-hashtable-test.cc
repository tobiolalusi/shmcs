#include <doctest/doctest.h>

#include "shmcs/chained-hashtable.hh"
#include <iostream>

TEST_SUITE("Chained hash table") {
  TEST_CASE("key insertion and bucket reading") {
    auto hashtable = shmcs::ChainedHashTable<uint32_t>{
        [](uint32_t k) -> size_t { return k; }, 3};
    hashtable.insert(1);
    hashtable.insert(2);
    hashtable.insert(3);
    hashtable.insert(4);
    CHECK_EQ(4, hashtable.read(1).front());
    CHECK_EQ(2, hashtable.read(2).front());
    CHECK_EQ(3, hashtable.read(0).front());
    CHECK_THROWS_AS(hashtable.read(4).front(), shmcs::bucket_not_found);
  }

  TEST_CASE("key insertion and key removal") {
    auto hashtable = shmcs::ChainedHashTable<uint32_t>{
        [](uint32_t k) -> size_t { return k; }, 3};
    hashtable.insert(0);
    hashtable.insert(1);
    hashtable.insert(1);
    hashtable.insert(4);
    CHECK_EQ(1, hashtable.remove(0));
    CHECK_EQ(2, hashtable.remove(1));
    CHECK_EQ(0, hashtable.remove(2));
    CHECK_EQ(0, hashtable.remove(3));
    CHECK_EQ(1, hashtable.remove(4));
  }
}
