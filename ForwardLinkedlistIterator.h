#ifndef __FORWARD_LINKEDLIST_ITERATOR_H__
#define __FORWARD_LINKEDLIST_ITERATOR_H__

#include "LinkedlistNode.h"

template <typename T>
class ForwardLinkedlistIterator {
private:
    using value_type = T;
    using Node       = LinkedlistNode<T>;
    using iterator   = ForwardLinkedlistIterator<T>;

    Node* m_pNode = nullptr;

public:
    // Constructors
    explicit ForwardLinkedlistIterator(Node* pNode = nullptr)
        : m_pNode(pNode) {}

    ForwardLinkedlistIterator(const iterator& other)
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
