#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include "LinkedlistNode.h"
#include "ForwardLinkedlistIterator.h"

template <typename T>
class LinkedList {
private:
    using value_type = T;
    using Node       = LinkedlistNode<value_type>;

    Node* m_pRoot = nullptr;

public:
    // Default constructor
    LinkedList() = default;

    using iterator = ForwardLinkedlistIterator<T>;

    iterator begin() { return iterator(m_pRoot); }
    iterator end()   { return iterator(nullptr); }

    LinkedList(const LinkedList& other) : m_pRoot(nullptr) {
        if (!other.m_pRoot) return; // other list is empty

        // Copy first node
        m_pRoot = new Node(other.m_pRoot->GetData(), other.m_pRoot->GetRef());

        // Copy remaining nodes
        Node* currOther = other.m_pRoot->GetNext();
        Node* currThis  = m_pRoot;

        while (currOther) {
            currThis->GetNextRef() = new Node(currOther->GetData(), currOther->GetRef());
            currThis  = currThis->GetNext();
            currOther = currOther->GetNext();
        }
    }

    // Move constructor
    LinkedList(LinkedList&& other) noexcept
        : m_pRoot(other.m_pRoot) {
        other.m_pRoot = nullptr;
    }

    // Destructor
    ~LinkedList() {
        clear();
    }

    // Copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* curr = other.m_pRoot;
            Node** tail = &m_pRoot;
            while (curr) {
                *tail = new Node(curr->GetData(), curr->GetRef());
                curr = curr->GetNext();
                tail = &((*tail)->GetNextRef());
            }
        }
        return *this;
    }

    // Move assignment
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            m_pRoot = other.m_pRoot;
            other.m_pRoot = nullptr;
        }
        return *this;
    }

    // Insert at head
    void insert(const value_type& elem, Ref ref) {
        m_pRoot = new Node(elem, ref, m_pRoot);
    }

    void clear() {
        while (m_pRoot) {
            Node* temp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete temp;
        }
    }

private:
    Node* get_root() const { return m_pRoot; }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const LinkedList<T>& obj) {
        auto pRoot = obj.get_root();
        while (pRoot) {
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }
};

#endif // __LINKEDLIST_H__