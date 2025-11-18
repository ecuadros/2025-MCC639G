#ifndef LINKED_LIST_TRAITS_NODE_H
#define LINKED_LIST_TRAITS_NODE_H

#include <utility>

template<typename T>
class LinkedListNode {
public:
    using value_type = T;
    using reference_type = T &;
    using const_reference_type = const T &;
    using pointer_type = T *;
    using const_pointer_type = const T *;
    using node_type = LinkedListNode<T>;
    using node_pointer = node_type *;

    LinkedListNode() noexcept
        : m_value{}, m_next(nullptr) {
    }

    explicit LinkedListNode(const value_type &info)
        : m_value(info), m_next(nullptr) {
    }

    explicit LinkedListNode(value_type &&info) noexcept
        : m_value(std::move(info)), m_next(nullptr) {
    }

    LinkedListNode(const LinkedListNode &) = delete;

    LinkedListNode &operator=(const LinkedListNode &) = delete;

    LinkedListNode(LinkedListNode &&other) noexcept
        : m_value(std::move(other.m_value)), m_next(std::exchange(other.m_next, nullptr)) {
    }

    LinkedListNode &operator=(LinkedListNode &&other) noexcept {
        if (this != &other) {
            m_value = std::move(other.m_value);
            m_next = std::exchange(other.m_next, nullptr);
        }
        return *this;
    }

    reference_type value() noexcept { return m_value; }
    const_reference_type value() const noexcept { return m_value; }

    node_pointer next() noexcept { return m_next; }
    const node_pointer next() const noexcept { return m_next; }

    void set_next(node_pointer next) noexcept { m_next = next; }

private:
    value_type m_value;
    node_pointer m_next;
};

#endif // LINKED_LIST_TRAITS_NODE_H