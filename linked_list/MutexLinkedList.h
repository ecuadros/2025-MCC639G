#ifndef __MUTEXLINKEDLIST_H__
#define __MUTEXLINKEDLIST_H__

#include <iostream>
#include <mutex>
#include "LinkedListNode.h"
#include "ForwardLinkedListIterator.h"

template <typename T>
class MutexLinkedList {
private:
    using value_type = T;
    using Node       = LinkedListNode<value_type>;

    Node* m_pRoot = nullptr;
    mutable std::mutex m_mutex;

public:
    using iterator = ForwardLinkedListIterator<T>;

    // Default constructor
    MutexLinkedList() = default;

    // Copy constructor
    MutexLinkedList(const MutexLinkedList& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        if (!other.m_pRoot) return;

        m_pRoot = new Node(other.m_pRoot->GetData(), other.m_pRoot->GetRef());
        Node* currOther = other.m_pRoot->GetNext();
        Node* currThis  = m_pRoot;

        while (currOther) {
            currThis->GetNextRef() = new Node(currOther->GetData(), currOther->GetRef());
            currThis  = currThis->GetNext();
            currOther = currOther->GetNext();
        }
    }

    // Move constructor
    MutexLinkedList(MutexLinkedList&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        other.m_pRoot = nullptr;
    }

    // Destructor
    ~MutexLinkedList() {
        clear();
    }

    // Copy assignment
    MutexLinkedList& operator=(const MutexLinkedList& other) {
        if (this != &other) {
            clear();
            std::scoped_lock lock(m_mutex, other.m_mutex);
            if (!other.m_pRoot) return *this;

            m_pRoot = new Node(other.m_pRoot->GetData(), other.m_pRoot->GetRef());
            Node* currOther = other.m_pRoot->GetNext();
            Node* currThis  = m_pRoot;

            while (currOther) {
                currThis->GetNextRef() = new Node(currOther->GetData(), currOther->GetRef());
                currThis  = currThis->GetNext();
                currOther = currOther->GetNext();
            }
        }
        return *this;
    }

    // Move assignment
    MutexLinkedList& operator=(MutexLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            std::scoped_lock lock(m_mutex, other.m_mutex);
            m_pRoot = other.m_pRoot;
            other.m_pRoot = nullptr;
        }
        return *this;
    }

    // Insert at head
    void insert(const value_type& elem, Ref ref) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pRoot = new Node(elem, ref, m_pRoot);
    }

    // Pop head
    bool pop(value_type& out) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pRoot) return false;
        Node* temp = m_pRoot;
        out = temp->GetData();
        m_pRoot = m_pRoot->GetNext();
        delete temp;
        return true;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (m_pRoot) {
            Node* temp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete temp;
        }
    }

    // Iterators (⚠️ iteration not safe if list is modified concurrently)
    iterator begin() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return iterator(m_pRoot);
    }

    iterator end() {
        return iterator(nullptr);
    }

private:
    Node* get_root() const { return m_pRoot; }

    // Debug print
    friend std::ostream& operator<<(std::ostream& os, const MutexLinkedList<T>& obj) {
        std::lock_guard<std::mutex> lock(obj.m_mutex);
        auto pRoot = obj.get_root();
        while (pRoot) {
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }
};

#endif // __MUTEXLINKEDLIST_H__
