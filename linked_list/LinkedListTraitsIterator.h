#ifndef LINKED_LIST_TRAITS_ITERATOR_H
#define LINKED_LIST_TRAITS_ITERATOR_H

#include "LinkedListTraitsNode.h"

template <typename T>
class LinkedListTraitsIterator {
public:
    typedef LinkedListTraitsNode<T> node_type;
    typedef node_type* node_pointer;
    typedef typename node_type::value_type value_type;
    typedef typename node_type::reference_type reference_type;

    LinkedListTraitsIterator(node_pointer ptr) : m_ptr(ptr) {}

    LinkedListTraitsIterator& operator++() {
        m_ptr = m_ptr->next();
        return *this;
    }

    reference_type operator*() {
        return m_ptr->value();
    }

    bool operator!=(const LinkedListTraitsIterator& other) const {
        return m_ptr != other.m_ptr;
    }

private:
    node_pointer m_ptr;
};

#endif // LINKED_LIST_TRAITS_ITERATOR_H
