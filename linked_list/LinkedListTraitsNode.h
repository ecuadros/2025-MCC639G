#ifndef LINKED_LIST_TRAITS_NODE_H
#define LINKED_LIST_TRAITS_NODE_H

template <typename T>
class LinkedListTraitsNode {
public:
    typedef T value_type;
    typedef T& reference_type;
    typedef const T& const_reference_type;
    typedef T* pointer_type;
    typedef const T* const_pointer_type;

    LinkedListTraitsNode(value_type info) : m_value(info), m_next(nullptr) {}

    reference_type value() { return m_value; }
    const_reference_type value() const { return m_value; }

    LinkedListTraitsNode* next() { return m_next; }
    const LinkedListTraitsNode* next() const { return m_next; }

    void set_next(LinkedListTraitsNode* next) { m_next = next; }

private:
    value_type m_value;
    LinkedListTraitsNode* m_next;
};

#endif // LINKED_LIST_TRAITS_NODE_H
