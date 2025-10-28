#ifndef LINKED_LIST_TRAITS_ITERATOR_H
#define LINKED_LIST_TRAITS_ITERATOR_H

#include "LinkedListTraitsNode.h"
#include <iterator>

template <typename T>
class LinkedListTraitsIterator {
public:
    using node_type        = LinkedListTraitsNode<T>;
    using node_pointer     = node_type*;
    using value_type       = typename node_type::value_type;
    using reference        = typename node_type::reference_type;
    using pointer          = value_type*;
    using iterator_category = std::forward_iterator_tag;
    using difference_type  = std::ptrdiff_t;

    explicit LinkedListTraitsIterator(node_pointer ptr = nullptr) noexcept
        : m_ptr(ptr) {}

    reference operator*() const noexcept {
        return m_ptr->value();
    }

    pointer operator->() const noexcept {
        return &m_ptr->value();
    }

    LinkedListTraitsIterator& operator++() noexcept {
        if (m_ptr)
            m_ptr = m_ptr->next();
        return *this;
    }

    LinkedListTraitsIterator operator++(int) noexcept {
        LinkedListTraitsIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(const LinkedListTraitsIterator& other) const noexcept {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const LinkedListTraitsIterator& other) const noexcept {
        return m_ptr != other.m_ptr;
    }

private:
    node_pointer m_ptr;
};

#endif // LINKED_LIST_TRAITS_ITERATOR_H
