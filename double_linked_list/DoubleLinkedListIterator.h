#ifndef __DLL_ITERATOR_H__
#define __DLL_ITERATOR_H__

#include "DoubleLinkedListNode.h"
#include <iterator>

template <typename T>
class DoubleLinkedListIterator {
private:
    using Node = DoubleLinkedListNode<T>;
    Node* m_node = nullptr;

public:
    using value_type        = T;
    using reference         = T&;
    using pointer           = T*;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;

    explicit DoubleLinkedListIterator(Node* node = nullptr) : m_node(node) {}

    // Operators
    reference operator*() { return m_node->GetDataRef(); }
    pointer   operator->(){ return &m_node->GetDataRef(); }

    bool operator==(const DoubleLinkedListIterator& other) const { return m_node == other.m_node; }
    bool operator!=(const DoubleLinkedListIterator& other) const { return m_node != other.m_node; }

    // ++it
    DoubleLinkedListIterator& operator++() {
        if (m_node) m_node = m_node->GetNext();
        return *this;
    }

    // it++
    DoubleLinkedListIterator operator++(int) {
        auto tmp(*this);
        ++(*this);
        return tmp;
    }

    // --it
    DoubleLinkedListIterator& operator--() {
        if (m_node) m_node = m_node->GetPrev();
        return *this;
    }

    // it--
    DoubleLinkedListIterator operator--(int) {
        auto tmp(*this);
        --(*this);
        return tmp;
    }

    Node* GetNode() const { return m_node; }
};

#endif // __DLL_ITERATOR_H__
