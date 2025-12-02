#ifndef DOUBLE_LINKED_LIST_TRAITS_H
#define DOUBLE_LINKED_LIST_TRAITS_H

#include <ostream>
#include <utility>
#include <mutex>
#include <functional>
#include "DoubleLinkedListNode.h"
#include "DoubleLinkedListIterator.h"

template<typename T>
struct DoubleLinkedListTraits {
    using value_type = T;
    using node_type = DoubleLinkedListNode<T>;
    using compare_fn = std::less<T>;
};

template<typename Traits>
class DoubleLinkedList {
public:
    using node_type = typename Traits::node_type;
    using node_pointer = node_type *;
    using value_type = typename Traits::value_type;

    using iterator = DoubleLinkedListForwardIterator<value_type>;
    using reverse_iterator = DoubleLinkedListBackwardIterator<value_type>;

    DoubleLinkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {
    }

    ~DoubleLinkedList() {
        clear();
    }

    DoubleLinkedList(const DoubleLinkedList &other)
        : m_head(nullptr), m_tail(nullptr), m_size(0) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        for (const auto &value: other)
            add(value);
    }

    DoubleLinkedList &operator=(const DoubleLinkedList &other) {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            for (const auto &value: other)
                add(value);
        }
        return *this;
    }

    DoubleLinkedList(DoubleLinkedList &&other) noexcept
        : m_head(std::exchange(other.m_head, nullptr)),
          m_tail(std::exchange(other.m_tail, nullptr)),
          m_size(std::exchange(other.m_size, 0)) {
    }

    DoubleLinkedList &operator=(DoubleLinkedList &&other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(m_mutex, other.m_mutex);
            clear();
            m_head = std::exchange(other.m_head, nullptr);
            m_tail = std::exchange(other.m_tail, nullptr);
            m_size = std::exchange(other.m_size, 0);
        }
        return *this;
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
    reverse_iterator rbegin() const noexcept { return reverse_iterator(m_tail); }
    reverse_iterator rend() const noexcept { return reverse_iterator(nullptr); }

    size_t size() const noexcept { return m_size; }

    friend std::ostream &operator<<(std::ostream &os, const DoubleLinkedList<Traits> &list) {
        std::lock_guard<std::mutex> lock(list.m_mutex);
        for (const auto &value: list)
            os << value << " ";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, DoubleLinkedList<Traits> &list) {
        value_type value;
        list.clear();
        while (is >> value)
            list.add(value);
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

#endif // DOUBLE_LINKED_LIST_TRAITS_H