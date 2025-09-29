#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include <iostream>
#include "DoubleLinkedListNode.h"
#include "DoubleLinkedListIterator.h"

template <typename T>
class DoubleLinkedList {
private:
    using Node = DoubleLinkedListNode<T>;
    Node* m_head = nullptr;
    Node* m_tail = nullptr;

public:
    using iterator = DoubleLinkedListIterator<T>;

    DoubleLinkedList() = default;

    ~DoubleLinkedList() { Clear(); }

    // Insert at head
    void PushFront(const T& data, Ref ref) {
        Node* newNode = new Node(data, ref, m_head, nullptr);
        if (m_head) m_head->PrevRef() = newNode;
        m_head = newNode;
        if (!m_tail) m_tail = newNode;
    }

    // Insert at tail
    void PushBack(const T& data, Ref ref) {
        Node* newNode = new Node(data, ref, nullptr, m_tail);
        if (m_tail) m_tail->NextRef() = newNode;
        m_tail = newNode;
        if (!m_head) m_head = newNode;
    }

    void Clear() {
        Node* curr = m_head;
        while (curr) {
            Node* next = curr->GetNext();
            delete curr;
            curr = next;
        }
        m_head = m_tail = nullptr;
    }

    // Iterators
    iterator begin() { return iterator(m_head); }
    iterator end()   { return iterator(nullptr); }

    iterator rbegin(){ return iterator(m_tail); }
    iterator rend()  { return iterator(nullptr); }

    // Debug print
    friend std::ostream& operator<<(std::ostream& os, const DoubleLinkedList<T>& list) {
        Node* curr = list.m_head;
        while (curr) {
            os << curr->GetData() << "(" << curr->GetRef() << ") ";
            curr = curr->GetNext();
        }
        return os;
    }
};

#endif // __DOUBLELINKEDLIST_H__
