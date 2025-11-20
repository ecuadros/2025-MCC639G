#ifndef __DOUBLE_LINKEDLIST_H__
#define __DOUBLE_LINKEDLIST_H__

#include <iostream>
#include <mutex>
#include "types.h"
#include "traits.h"

// ------------------------------
// NODO DE LA LISTA DOBLE
// ------------------------------
template <typename Traits>
class DLLNode {
private:
    using value_type = typename Traits::value_type;
    using Node       = DLLNode<Traits>;

    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    DLLNode(value_type &elem, Ref ref, Node *pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext) {}

    value_type   GetData()     { return m_data; }
    value_type  &GetDataRef()  { return m_data; }
    Ref          GetRef()      { return m_ref; }
    Node        *GetNext()     { return m_pNext; }
    Node        *&GetNextRef() { return m_pNext; }
    void         SetNext(Node *pNext) { m_pNext = pNext; }

    Node        *GetPrev()     { return m_pPrev; }
    Node        *&GetPrevRef() { return m_pPrev; }
    void         SetPrev(Node *pPrev) { m_pPrev = pPrev; }
};

// ------------------------------
// ITERADOR FORWARD
// ------------------------------
template <typename Container>
class forward_double_linkedlist_iterator {
private:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    using iterator   = forward_double_linkedlist_iterator<Container>;

    Container *m_pList = nullptr;
    Node      *m_pNode = nullptr;

public:
    forward_double_linkedlist_iterator(Container *pList, Node *pNode)
        : m_pList(pList), m_pNode(pNode) {}

    forward_double_linkedlist_iterator(iterator &other)
        : m_pList(other.m_pList), m_pNode(other.m_pNode) {}

    bool operator==(iterator other) { return m_pList == other.m_pList && m_pNode == other.m_pNode; }
    bool operator!=(iterator other) { return !(*this == other); }

    iterator operator++() {
        if (m_pNode) m_pNode = m_pNode->GetNext();
        return *this;
    }

    value_type &operator*() { return m_pNode->GetDataRef(); }
};

// ------------------------------
// ITERADOR BACKWARD
// ------------------------------
template <typename Container>
class backward_double_linkedlist_iterator {
private:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    using iterator   = backward_double_linkedlist_iterator<Container>;

    Container *m_pList = nullptr;
    Node      *m_pNode = nullptr;

public:
    backward_double_linkedlist_iterator(Container *pList, Node *pNode)
        : m_pList(pList), m_pNode(pNode) {}

    backward_double_linkedlist_iterator(iterator &other)
        : m_pList(other.m_pList), m_pNode(other.m_pNode) {}

    bool operator==(iterator other) { return m_pList == other.m_pList && m_pNode == other.m_pNode; }
    bool operator!=(iterator other) { return !(*this == other); }

    iterator operator++() {
        if (m_pNode) m_pNode = m_pNode->GetPrev();
        return *this;
    }

    value_type &operator*() { return m_pNode->GetDataRef(); }
};

// ------------------------------
// LISTA DOBLEMENTE ENLAZADA
// ------------------------------
template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type         = typename Traits::value_type;
    using Func               = typename Traits::Func;
    using Node               = DLLNode<Traits>;
    using Container          = CDoubleLinkedList<Traits>;
    using forward_iterator   = forward_double_linkedlist_iterator<Container>;
    using backward_iterator  = backward_double_linkedlist_iterator<Container>;

private:
    Node   *m_pRoot = nullptr;
    Node   *m_pTail = nullptr;
    size_t  m_nElem = 0;
    Func    m_fCompare;
    mutable std::mutex m_mtx; // Concurrencia

public:
    // Constructores y destructor
    CDoubleLinkedList();
    CDoubleLinkedList(CDoubleLinkedList &other);
    CDoubleLinkedList(CDoubleLinkedList &&other);
    virtual ~CDoubleLinkedList();

    // Inserción
    void Insert(value_type &elem, Ref ref);

    // Iteradores
    forward_iterator begin()  { return forward_iterator(this, m_pRoot); }
    forward_iterator end()    { return forward_iterator(this, nullptr); }
    backward_iterator rbegin(){ return backward_iterator(this, m_pTail); }
    backward_iterator rend()  { return backward_iterator(this, nullptr); }

    // Persistencia
    std::ostream &Write(std::ostream &os) { return os << *this; }
    std::istream &Read(std::istream &is);

    // Operador de impresión
    friend std::ostream& operator<<(std::ostream &os, CDoubleLinkedList<Traits> &obj){
        std::lock_guard<std::mutex> lock(obj.m_mtx);
        auto pRoot = obj.GetRoot();
        while (pRoot) {
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }

private:
    void InternalInsert(Node *&rParent, value_type &elem, Ref ref);
    Node *GetRoot() { return m_pRoot; }
};

// ------------------------------
// IMPLEMENTACIONES
// ------------------------------

// Constructor por defecto
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList() {}

// Constructor por copia
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &other) {
    std::lock_guard<std::mutex> lock(other.m_mtx);
    Node* cur = other.m_pRoot;
    while (cur) {
        value_type tmp = cur->GetDataRef();
        Insert(tmp, cur->GetRef());
        cur = cur->GetNext();
    }
}

// Constructor por movimiento
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &&other) {
    std::lock_guard<std::mutex> lock(other.m_mtx);
    m_pRoot    = std::move(other.m_pRoot);
    m_pTail    = std::move(other.m_pTail);
    m_nElem    = std::move(other.m_nElem);
    m_fCompare = std::move(other.m_fCompare);
    other.m_pRoot = nullptr;
    other.m_pTail = nullptr;
    other.m_nElem = 0;
}

// Destructor
template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList() {
    std::lock_guard<std::mutex> lock(m_mtx);
    Node* cur = m_pRoot;
    while (cur) {
        Node* next = cur->GetNext();
        delete cur;
        cur = next;
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;
    m_nElem = 0;
}

// Inserción ordenada
template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(value_type &elem, Ref ref) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInsert(m_pRoot, elem, ref);
}

// Inserción recursiva con enlace doble
template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, value_type &elem, Ref ref) {
    if (!rParent || m_fCompare(elem, rParent->GetDataRef())) {
        Node *pNew = rParent = new Node(elem, ref, rParent);
        if (!pNew->GetNext()) m_pTail = pNew;

        Node *pNext = pNew->GetNext();
        if (pNext) {
            pNew->SetPrev(pNext->GetPrev());
            pNext->SetPrev(pNew);
        }
        m_nElem++;
        return;
    }
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

// Lectura desde stream
template <typename Traits>
std::istream &CDoubleLinkedList<Traits>::Read(std::istream &is) {
    std::lock_guard<std::mutex> lock(m_mtx);
    size_t n;
    is >> n;
    for (size_t i = 0; i < n; ++i) {
        value_type elem;
        Ref ref;
        is >> elem >> ref;
        Insert(elem, ref);
    }
    return is;
}
void DemoDoubleLinkedList();