#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <mutex>
#include <utility>
#include "types.h"

// ============================================================================
// TRAITS
// ============================================================================
template <typename T, typename _Func>
struct LLinkedListTraits {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct LLinkedListAsc :
    public LLinkedListTraits<T, std::less<T>> {
};

template <typename T>
struct LLinkedListDesc :
    public LLinkedListTraits<T, std::greater<T>> {
};

// ============================================================================
// NODE
// ============================================================================
template <typename Traits>
class LLNode {
private:
    using value_type = typename Traits::value_type;
    using Node       = LLNode<Traits>;

    value_type m_data;
    Ref        m_ref;
    Node*      m_pNext;

public:
    LLNode(value_type &elem, Ref ref, Node *next = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(next) {}

    value_type   GetData()    const { return m_data; }
    value_type&  GetDataRef()       { return m_data; }
    Ref          GetRef()     const { return m_ref; }
    Node*        GetNext()    const { return m_pNext; }
    Node*&       GetNextRef()       { return m_pNext; }
};

// ============================================================================
// FORWARD ITERATOR
// ============================================================================
template <typename Container>
class forward_linkedlist_iterator {
private:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    Node *m_pNode = nullptr;

public:
    forward_linkedlist_iterator(Node *node)
        : m_pNode(node)
    {}

    forward_linkedlist_iterator(const forward_linkedlist_iterator &other)
        : m_pNode(other.m_pNode)
    {}

    bool operator==(const forward_linkedlist_iterator &other) const {
        return m_pNode == other.m_pNode;
    }

    bool operator!=(const forward_linkedlist_iterator &other) const {
        return !(*this == other);
    }

    forward_linkedlist_iterator& operator++() {
        if (m_pNode)
            m_pNode = m_pNode->GetNext();
        return *this;
    }

    value_type& operator*() const {
        return m_pNode->GetDataRef();
    }
};

// ============================================================================
// LINKED LIST
// ============================================================================
template <typename Traits>
class CLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Func       = typename Traits::Func;
    using Node       = LLNode<Traits>;
    using Container  = CLinkedList<Traits>;

    using iterator       = forward_linkedlist_iterator<Container>;
    using const_iterator = forward_linkedlist_iterator<const Container>;

private:
    Node*            m_pRoot = nullptr;
    size_t           m_nElem = 0;
    Func             m_fCompare;
    mutable std::mutex m_mutex;

private:
    // Internal insert
    void InternalInsert(Node *&parent, value_type &elem, Ref ref) {
        if (!parent || m_fCompare(elem, parent->GetDataRef())) {
            parent = new Node(elem, ref, parent);
            m_nElem++;
            return;
        }
        InternalInsert(parent->GetNextRef(), elem, ref);
    }

    Node* GetRoot() const { return m_pRoot; }

public:
    // ============================================================================
    // CONSTRUCTORS
    // ============================================================================
    CLinkedList() {}

    // Copy Constructor
    CLinkedList(const CLinkedList &other) {
        std::scoped_lock lock(m_mutex, other.m_mutex);

        m_pRoot    = nullptr;
        m_nElem    = 0;
        m_fCompare = other.m_fCompare;

        if (!other.m_pRoot)
            return;

        Node *pOther = other.m_pRoot;
        m_pRoot = new Node(pOther->GetDataRef(), pOther->GetRef());
        Node *current = m_pRoot;
        pOther = pOther->GetNext();
        m_nElem++;

        while (pOther) {
            current->GetNextRef() = new Node(pOther->GetDataRef(), pOther->GetRef());
            current = current->GetNext();
            pOther = pOther->GetNext();
            m_nElem++;
        }
    }

    // Move Constructor
    CLinkedList(CLinkedList &&other) {
        std::scoped_lock lock(m_mutex, other.m_mutex);

        m_pRoot    = std::exchange(other.m_pRoot, nullptr);
        m_nElem    = std::exchange(other.m_nElem, 0);
        m_fCompare = std::move(other.m_fCompare);
    }

    // ============================================================================
    // DESTRUCTOR (SEGURO)
    // ============================================================================
    ~CLinkedList() {
        Node *current = m_pRoot;
        while (current != nullptr) {
            Node *next = current->GetNext();
            delete current;
            current = next;
        }
        m_pRoot = nullptr;
        m_nElem = 0;
    }

    // ============================================================================
    // INSERT
    // ============================================================================
    void Insert(value_type &elem, Ref ref) {
        std::scoped_lock lock(m_mutex);
        InternalInsert(m_pRoot, elem, ref);
    }

    // ============================================================================
    // READ / WRITE
    // ============================================================================
    value_type ReadAt(size_t index) const {
        std::scoped_lock lock(m_mutex);

        Node *p = m_pRoot;
        while (p && index--)
            p = p->GetNext();

        if (!p)
            throw std::out_of_range("Index out of range");

        return p->GetData();
    }

    void WriteAt(size_t index, const value_type &value) {
        std::scoped_lock lock(m_mutex);

        Node *p = m_pRoot;
        while (p && index--)
            p = p->GetNext();

        if (!p)
            throw std::out_of_range("Index out of range");

        p->GetDataRef() = value;
    }

    // ============================================================================
    // ITERATORS
    // ============================================================================
    iterator begin() {
        std::scoped_lock lock(m_mutex);
        return iterator(m_pRoot);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const {
        std::scoped_lock lock(m_mutex);
        return const_iterator(m_pRoot);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    // ============================================================================
    // TOSTRING
    // ============================================================================
    friend std::ostream& operator<<(std::ostream &os, const CLinkedList<Traits> &obj) {
        Node *p = obj.GetRoot();
        while (p) {
            os << p->GetData() << "(" << p->GetRef() << ") ";
            p = p->GetNext();
        }
        return os;
    }
};

// Demo
void DemoLinkedList();

#endif // __LINKEDLIST_H__