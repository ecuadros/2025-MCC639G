#ifndef __DOUBLELINKEDLISTNODE_H__
#define __DOUBLELINKEDLISTNODE_H__

// For now, keep Ref as int
using Ref = int;

template <typename T>
class DoubleLinkedListNode {
private:
    T m_data;
    Ref m_ref;
    DoubleLinkedListNode* m_next = nullptr;
    DoubleLinkedListNode* m_prev = nullptr;

public:
    using Node = DoubleLinkedListNode<T>;

    DoubleLinkedListNode(const T& data, Ref ref, Node* next = nullptr, Node* prev = nullptr)
        : m_data(data), m_ref(ref), m_next(next), m_prev(prev) {}

    T&       GetDataRef()       { return m_data; }
    const T& GetData() const    { return m_data; }
    Ref      GetRef() const     { return m_ref; }

    Node*       GetNext()       { return m_next; }
    const Node* GetNext() const { return m_next; }

    Node*       GetPrev()       { return m_prev; }
    const Node* GetPrev() const { return m_prev; }

    Node*& NextRef() { return m_next; }
    Node*& PrevRef() { return m_prev; }
};

#endif // __DOUBLELINKEDLISTNODE_H__
