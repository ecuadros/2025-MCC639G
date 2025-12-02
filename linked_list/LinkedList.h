#ifndef LINKED_LIST_TRAITS_H
#define LINKED_LIST_TRAITS_H

#include <ostream>
#include <istream>
#include <mutex>
#include <utility>
#include <functional> // For std::less
#include "LinkedListNode.h"
#include "LinkedListIterator.h"

template<typename T>
struct LinkedListTraits {
    using value_type = T;
    using node_type = LinkedListNode<T>;
    using iterator_type = LinkedListIterator<T>;
    using compare_fn = std::less<T>;
};

template<typename Traits>
class LinkedList {
public:
    using node_type = typename Traits::node_type;
    using node_pointer = node_type *;
    using value_type = typename Traits::value_type;
    using iterator = typename Traits::iterator_type;

    using reference_type = value_type &;
    using const_reference_type = const value_type &;

    LinkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {
    }

    LinkedList(const LinkedList &other)
        : m_head(nullptr), m_tail(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);

        for (const auto &value: other)
            add(value);
    }

    LinkedList &operator=(const LinkedList &other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            for (const auto &value: other)
                add(value);
        }
        return *this;
    }

    LinkedList(LinkedList &&other) noexcept
        : m_head(std::exchange(other.m_head, nullptr)),
          m_tail(std::exchange(other.m_tail, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {
    }

    LinkedList &operator=(LinkedList &&other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            m_head = std::exchange(other.m_head, nullptr);
            m_tail = std::exchange(other.m_tail, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    ~LinkedList() {
        clear();
        std::cout << "[Debug] LinkedListTraits destructor called." << std::endl;
    }

    void add(value_type info) {
        std::lock_guard<std::mutex> lock(m_mutex);
        node_pointer new_node = new node_type(std::move(info));

        if (!m_head) {
            m_head = new_node;
            m_tail = new_node;
        } else {
            m_tail->set_next(new_node);
            m_tail = new_node;
        }
        ++m_size;
    }

    iterator begin() const noexcept { return iterator(m_head); }
    iterator end() const noexcept { return iterator(nullptr); }

    size_t size() const noexcept { return m_size; }

    friend std::ostream &operator<<(std::ostream &os, const LinkedList<Traits> &list) {
        std::lock_guard<std::mutex> lock(list.m_mutex);
        for (const auto &value: list)
            os << value << " ";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, LinkedList<Traits> &list) {
        value_type value;
        while (is >> value)
            list.add(std::move(value));
        return is;
    }

private:
    void clear() noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        node_pointer current = m_head;
        while (current) {
            node_pointer next = current->next();
            delete current;
            current = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    mutable std::mutex m_mutex;
    node_pointer m_head;
    node_pointer m_tail;
    size_t m_size;
};

#endif // LINKED_LIST_TRAITS_H