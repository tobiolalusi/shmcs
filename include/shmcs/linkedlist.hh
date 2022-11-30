#ifndef SHMCS_LINKEDLIST_HH
#define SHMCS_LINKEDLIST_HH

#include <optional>
#include <list>

namespace shmcs {

template<typename T>
struct node_t {
  std::optional<T> data;
  node_t<T>* next;
};

template<typename E>
class LinkedList {
  public:
  LinkedList() {
    head.data = std::nullopt;
    head.next = tail;
  };

  /// appends element to linked list
  auto append(E e) -> void {
    if (head.data.has_value()) {
      auto node = new node_t<E>;
      node->data = e;
      node->next = &head;
      tail->next = node;
      tail = node;
    } else {
      head.data = e;
    }
  }

  /// creates a single list of all the node data in the linked list
  auto getall() -> std::list<E> {
    std::list<E> list;
    auto node = &head;
    do {
      list.push_back(node->data);
      node = node->next;
    } while (&node != tail);
    return list;
  }

  /// searches linked list for an element and returns pointer to that node
  auto find(E e) -> node_t<E>* {
    auto node = &head;
    do {
      if (node->data == e) return node;
      node = node->next;
    } while (&node != tail);
    return nullptr;
  }

  /// removes a node from the linked list
  auto remove(E e) -> void {
    delete find(e);
  }

  private:
  node_t<E> head{};
  node_t<E>* tail = &head;
};

} // namespace shmcs

#endif // SHMCS_LINKEDLIST_HH
