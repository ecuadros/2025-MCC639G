#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "types.h"
#include <mutex>
#include <iostream>

// ------------------------------
// NODO DE LA LISTA ENLAZADA
// ------------------------------
template <typename T>
class LLNode {
private:
    using Type = T;
    using Node = LLNode<T>;
    Type m_data;       // Dato almacenado
    Ref  m_ref;        // Referencia adicional
    Node *m_pNext = nullptr; // Puntero al siguiente nodo

public:
    // Constructor del nodo
    LLNode(Type &elem, Ref ref, LLNode<T> *pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext) {}

    // Métodos de acceso
    Type   GetData()     { return m_data; }
    Type  &GetDataRef()  { return m_data; }
    Node  *GetNext()     { return m_pNext; }
    Node  *&GetNextRef() { return m_pNext; }
    Ref    GetRef()      { return m_ref; }
};

// ------------------------------
// CLASE PRINCIPAL: CLinkedList
// ------------------------------
template <typename T>
class CLinkedList {
private:
    using Type = T;
    using Node = LLNode<Type>;
    Node *m_pHead = nullptr;           // Puntero al primer nodo
    mutable std::mutex m_mtx;          // Protección para concurrencia

public:
    // ------------------------------
    // CONSTRUCTORES Y DESTRUCTOR
    // ------------------------------

    // Constructor por defecto
    CLinkedList();

    // Constructor copia (deep copy)
    CLinkedList(CLinkedList &other);

    // Constructor movimiento (transfiere propiedad)
    CLinkedList(CLinkedList &&other);

    // Destructor (libera memoria)
    virtual ~CLinkedList();

    // ------------------------------
    // MÉTODOS PRINCIPALES
    // ------------------------------

    // Inserta un elemento ordenado
    void Insert(Type &elem, Ref ref);

    // Lectura desde stream
    void Read(std::istream &is);

    // Escritura hacia stream
    void Write(std::ostream &os) const;

    // ------------------------------
    // ITERADORES
    // ------------------------------
    class iterator {
    public:
        iterator(Node* p = nullptr) : m_cur(p) {}
        Type& operator*() const { return m_cur->GetDataRef(); }
        iterator& operator++() { m_cur = m_cur->GetNext(); return *this; }
        bool operator!=(const iterator& rhs) const { return m_cur != rhs.m_cur; }
    private:
        Node* m_cur;
    };

    iterator begin() { return iterator(m_pHead); }
    iterator end()   { return iterator(nullptr); }

private:
    // Inserción recursiva
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);

    // Clona lista (usado por constructor copia)
    static Node* CloneList(const Node* srcHead);

    // Libera lista (usado por destructor)
    static void ClearList(Node* &head);
};

// ------------------------------
// IMPLEMENTACIONES
// ------------------------------

// Constructor por defecto
template <typename T>
CLinkedList<T>::CLinkedList() : m_pHead(nullptr) {}

// Constructor copia
template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &other) {
    std::lock_guard<std::mutex> lock(other.m_mtx); // Protege acceso concurrente
    m_pHead = CloneList(other.m_pHead);            // Clona la lista nodo por nodo
}

// Constructor movimiento
template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &&other) {
    std::lock_guard<std::mutex> lock(other.m_mtx); // Protege acceso concurrente
    m_pHead = other.m_pHead;                       // Transfiere propiedad
    other.m_pHead = nullptr;                       // Deja la lista original vacía
}

// Destructor
template <typename T>
CLinkedList<T>::~CLinkedList() {
    std::lock_guard<std::mutex> lock(m_mtx);       // Protege acceso concurrente
    ClearList(m_pHead);                            // Libera todos los nodos
}

// Inserción ordenada
template <typename T>
void CLinkedList<T>::Insert(Type &elem, Ref ref) {
    std::lock_guard<std::mutex> lock(m_mtx);       // Protege acceso concurrente
    InternalInsert(m_pHead, elem, ref);
}

// Inserción recursiva
template <typename T>
void CLinkedList<T>::InternalInsert(Node *&rParent, Type &elem, Ref ref) {
    if (!rParent || elem < rParent->GetDataRef()) {
        rParent = new Node(elem, ref, rParent);
        return;
    }
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

// Clonación de lista (usado por constructor copia)
template <typename T>
typename CLinkedList<T>::Node* CLinkedList<T>::CloneList(const Node* srcHead) {
    if (!srcHead) return nullptr;

    const Node* cur = srcHead;
    Node* newHead = nullptr;
    Node* tail = nullptr;

    while (cur) {
        Type tmp = cur->GetDataRef();              // Copia el dato
        Node* nn = new Node(tmp, cur->GetRef());   // Crea nuevo nodo

        if (!newHead) {
            newHead = nn;                          // Primer nodo
            tail = nn;
        } else {
            tail->GetNextRef() = nn;               // Enlaza al final
            tail = nn;
        }

        cur = cur->GetNext();                      // Avanza al siguiente
    }

    return newHead;
}

// Liberación de lista (usado por destructor)
template <typename T>
void CLinkedList<T>::ClearList(Node* &head) {
    Node* cur = head;
    while (cur) {
        Node* nxt = cur->GetNext();
        delete cur;
        cur = nxt;
    }
    head = nullptr;
}

// Lectura desde stream
template <typename T>
void CLinkedList<T>::Read(std::istream &is) {
    std::lock_guard<std::mutex> lock(m_mtx);       // Protege acceso concurrente
    ClearList(m_pHead);                            // Limpia lista actual

    std::size_t n;
    is >> n;
    for (std::size_t i = 0; i < n; ++i) {
        Type elem;
        Ref ref;
        is >> elem >> ref;
        Insert(elem, ref);
    }
}

// Escritura hacia stream
template <typename T>
void CLinkedList<T>::Write(std::ostream &os) const {
    std::lock_guard<std::mutex> lock(m_mtx);       // Protege acceso concurrente

    Node* cur = m_pHead;
    std::size_t count = 0;
    while (cur) {
        ++count;
        cur = cur->GetNext();
    }

    os << count << '\n';
    cur = m_pHead;
    while (cur) {
        os << cur->GetDataRef() << ' ' << cur->GetRef() << '\n';
        cur = cur->GetNext();
    }
}

// Demo para pruebas
void DemoLinkedList();

#endif // __LINKEDLIST_H__