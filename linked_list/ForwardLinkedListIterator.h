#ifndef __FORWARD_LINKEDLIST_ITERATOR_H__
#define __FORWARD_LINKEDLIST_ITERATOR_H__

#include "LinkedListNode.h"

template <typename T>
class ForwardLinkedListIterator {
private:
    using value_type = T;
    using Node       = LinkedListNode<T>;
    using iterator   = ForwardLinkedListIterator<T>;

    Node* m_pNode = nullptr;

public:
    // Constructors
    explicit ForwardLinkedListIterator(Node* pNode = nullptr)
        : m_pNode(pNode) {}

    ForwardLinkedListIterator(const iterator& other)
        : m_pNode(other.m_pNode) {}

    // Equality
    bool operator==(const iterator& other) const { return m_pNode == other.m_pNode; }
    bool operator!=(const iterator& other) const { return m_pNode != other.m_pNode; }

    // Pre-increment
    iterator& operator++() {
        if (m_pNode) m_pNode = m_pNode->GetNext();
        return *this;
    }

    // Post-increment
    iterator operator++(int) {
        iterator temp(*this);
        ++(*this);
        return temp;
    }

    // Dereference
    value_type& operator*() { return m_pNode->GetDataRef(); }
    value_type* operator->() { return &m_pNode->GetDataRef(); }
};

#endif // __FORWARD_LINKEDLIST_ITERATOR_H__
