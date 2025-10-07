#ifndef DOUBLE_LINKED_LIST_TRAITS_ITERATOR_H
#define DOUBLE_LINKED_LIST_TRAITS_ITERATOR_H

#include "DoubleLinkedListTraitsNode.h"

template <typename T>
class DoubleLinkedListTraitsIterator {
public:
    typedef DoubleLinkedListTraitsNode<T> node_type;
    typedef node_type* node_pointer;
    typedef typename node_type::value_type value_type;
    typedef typename node_type::reference_type reference_type;

    DoubleLinkedListTraitsIterator(node_pointer ptr) : m_ptr(ptr) {}

    reference_type operator*() {
        return m_ptr->value();
    }

    DoubleLinkedListTraitsIterator& operator++() {
        if (m_ptr) m_ptr = m_ptr->next();
        return *this;
    }

    DoubleLinkedListTraitsIterator& operator--() {
        if (m_ptr) m_ptr = m_ptr->prev();
        return *this;
    }

    bool operator!=(const DoubleLinkedListTraitsIterator& other) const {
        return m_ptr != other.m_ptr;
    }

private:
    node_pointer m_ptr;
};

#endif // DOUBLE_LINKED_LIST_TRAITS_ITERATOR_H
