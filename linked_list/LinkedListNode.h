#ifndef __LINKEDLISTNODE_H__
#define __LINKEDLISTNODE_H__

// Assuming Ref is just an int for now
using Ref = int;

template <typename T>
class LinkedListNode {
private:
    using value_type = T;
    using Node       = LinkedListNode<T>;

    value_type m_data;
    Ref        m_ref;
    Node*      m_pNext = nullptr;

public:
    LinkedListNode(const value_type& elem, Ref ref, Node* pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext) {}

    const value_type& GetData() const { return m_data; }
    value_type&       GetDataRef()    { return m_data; }

    Ref   GetRef()  const { return m_ref; }

    Node*       GetNext()       { return m_pNext; }
    const Node* GetNext() const { return m_pNext; }

    Node*& GetNextRef() { return m_pNext; }
};

#endif // __LINKEDLISTNODE_H__
