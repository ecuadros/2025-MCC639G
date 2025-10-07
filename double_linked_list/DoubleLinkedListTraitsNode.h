#ifndef DOUBLE_LINKED_LIST_TRAITS_NODE_H
#define DOUBLE_LINKED_LIST_TRAITS_NODE_H

#include <cstddef>

template <typename T>
class DoubleLinkedListTraitsNode {
public:
    typedef T value_type;
    typedef T& reference_type;
    typedef const T& const_reference_type;

    DoubleLinkedListTraitsNode(value_type info) 
        : m_value(info), m_next(nullptr), m_prev(nullptr) {}

    reference_type value() { return m_value; }
    const_reference_type value() const { return m_value; }

    DoubleLinkedListTraitsNode* next() { return m_next; }
    const DoubleLinkedListTraitsNode* next() const { return m_next; }
    void set_next(DoubleLinkedListTraitsNode* next) { m_next = next; }

    DoubleLinkedListTraitsNode* prev() { return m_prev; }
    const DoubleLinkedListTraitsNode* prev() const { return m_prev; }
    void set_prev(DoubleLinkedListTraitsNode* prev) { m_prev = prev; }

private:
    value_type m_value;
    DoubleLinkedListTraitsNode* m_next;
    DoubleLinkedListTraitsNode* m_prev;
};

#endif // DOUBLE_LINKED_LIST_TRAITS_NODE_H
