#ifndef __DOUBLE_LINKEDLIST_H__
#define __DOUBLE_LINKEDLIST_H__
#include <iostream>
#include "types.h"
#include "traits.h"

template <typename Traits>
class DLLNode{
private:
    using    value_type = typename Traits::value_type;
    using    Node       = DLLNode<Traits>;

    // Fields go here
    value_type          m_data;
    Ref                 m_ref;
    Node               *m_pNext = nullptr;
    Node               *m_pPrev = nullptr;

public:
    DLLNode(value_type &elem, Ref ref, Node *pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext){
    }
    value_type   GetData()    { return m_data;     }
    value_type  &GetDataRef() { return m_data;     }
    Ref    GetRef()     { return m_ref;      }
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
    // Diff
    void   SetNext(Node *pNext){    m_pNext = pNext; }

    // Particular para la double LinkedList
    Node * GetPrev()    { return m_pPrev;    }
    Node *&GetPrevRef() { return m_pPrev;    }
    // Diff
    void   SetPrev(Node *pPrev){    m_pPrev = pPrev; }
};

// ============================================================================
// ITERADOR UNICO - Un solo iterador para forward y backward
// Parámetro de plantilla:
//   - IsForward = true:  ++ avanza con GetNext() (forward)
//   - IsForward = false: ++ avanza con GetPrev() (backward)

template <typename Container, bool IsForward = true>
class double_linkedlist_iterator{
private:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    using iterator   = double_linkedlist_iterator<Container, IsForward>;

    Container *m_pList = nullptr;
    Node      *m_pNode = nullptr;

public:
    double_linkedlist_iterator(Container *pList, Node *pNode)
            : m_pList(pList), m_pNode(pNode){}
    double_linkedlist_iterator(iterator &other)
            : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
    
    bool operator==(iterator other){ return m_pList == other.m_pList && m_pNode == other.m_pNode; }
    bool operator!=(iterator other){ return !(*this == other); }
    
    // operator++: La dirección depende del parámetro IsForward
    // - Si IsForward=true:  usa GetNext() (avanza hacia adelante)
    // - Si IsForward=false: usa GetPrev() (avanza hacia atrás)
    iterator operator++(){ 
        if(m_pNode){
            if constexpr (IsForward)
                m_pNode = m_pNode->GetNext();
            else
                m_pNode = m_pNode->GetPrev();
        }
        return *this;
    }
    
    value_type &operator*(){ return m_pNode->GetDataRef(); }
};

// TODO Agregar control de concurrencia

// TODO Agregar que sea ascendente o descendente con el mismo codigo
template <typename Traits>
class CDoubleLinkedList{
public:
    using value_type = typename Traits::value_type; 
    using Func       = typename Traits::Func;
    using Node       = DLLNode<Traits>; 
    using Container  = CDoubleLinkedList<Traits>;
    using forward_iterator   = double_linkedlist_iterator<Container, true>;  // Ascendente
    using backward_iterator  = double_linkedlist_iterator<Container, false>;  // Descendente
    
private:
    Node   *m_pRoot = nullptr;
    Node   *m_pTail = nullptr;
    size_t m_nElem = 0;
    Func   m_fCompare;

// ========================================================================
// CONCURRENCIA: mutex para acceso exclusivo

    mutable std::mutex m_mutex;
public:
    // Constructor
    CDoubleLinkedList();
    CDoubleLinkedList(CDoubleLinkedList &other);

    // TODO: Done
    CDoubleLinkedList(CDoubleLinkedList &&other);

    // Destructor seguro
    virtual ~CDoubleLinkedList();

    void Insert(value_type &elem, Ref ref);
private:
    void InternalInsert(Node *&rParent, value_type &elem, Ref ref);
    Node *GetRoot()    {    return m_pRoot;     };

public:
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); }; 
    forward_iterator end()  { return forward_iterator(this, nullptr); } 

    // TODO: verifricar donde debe comenzar apuntando el iterator reverso
    backward_iterator rbegin(){ 
        std::lock_guard<std::mutex> lock(m_mutex);
        return backward_iterator(this, m_pTail); };
    backward_iterator rend()  { return backward_iterator(this, nullptr); } 

    friend std::ostream& operator<<(std::ostream &os, CDoubleLinkedList<Traits> &obj){
        std::lock_guard<std::mutex> lock(obj.m_mutex);
        auto pRoot = obj.GetRoot();
        while( pRoot ){
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }
public:
    // Persistence
    std::ostream &Write(std::ostream &os) { 
        std::lock_guard<std::mutex> lock(m_mutex);
        return os << *this; }
 
// ========================================================================
// READ
    // TODO: Read (istream &is)
    std::istream &Read (std::istream &is) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, elem, ref);

    };
};

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(value_type &elem, Ref ref){
    InternalInsert(m_pRoot, elem, ref);
}

// TODO: Agregar el enlace para el Prev()
template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, value_type &elem, Ref ref){
    if( !rParent || m_fCompare(elem, rParent->GetDataRef()) ){

        Node *pNew = rParent = new Node(elem, ref, rParent);
        if( !pNew->GetNext() ) // Final de la lista
            m_pTail = pNew;

        // Puente hacia atras
        Node *pNext = pNew->GetNext();
        if( pNext ){ // Hay algo a continuacion
            pNew ->SetPrev( pNext->GetPrev() );
            pNext->SetPrev( pNew ); 
        }
        m_nElem++;
        return;
    }
    // Tail recursion
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

// Constructor por defecto
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(){}

// ============================================================================
// CONSTRUCTOR COPIA: Crea una copia profunda de otra lista

// TODO Constructor por copia
//      Hacer loop copiando cada elemento
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &other){
        // Lock sobre la lista origen
    std::lock_guard<std::mutex> lock(other.m_mutex);
 
    // Recorrer la lista original y copiar cada nodo
    Node *pNode = other.m_pRoot;
    while(pNode){
        // Insertar copia del dato y su referencia
        // Nota: No usamos Insert() aquí para evitar deadlock,
        // llamamos directamente a InternalInsert()
        value_type val = pNode->GetData();
        Ref         ref = pNode->GetRef();
        InternalInsert(m_pRoot, val, ref); // Usamos InternalInsert() para evitar deadlock
        pNode = pNode->GetNext();
    }
}

// Move Constructor
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &&other){
    m_pRoot    = std::move(other.m_pRoot);
    m_pTail    = std::move(other.m_pTail);
    m_nElem    = std::move(other.m_nElem);
    m_fCompare = std::move(other.m_fCompare);

    // Invalidar objeto origen
    other.m_pRoot = nullptr;
    other.m_pTail = nullptr;
    other.m_nElem = 0;

}

// ============================================================================
// DESTRUCTOR: Libera toda la memoria
// TODO: Implementar y liberar la memoria de cada Node
template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList()
{
    Node *pNode = m_pRoot;
    while(pNode){
        Node *pNext = pNode->GetNext();
        delete pNode;
        pNode = pNext;
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;  
    n_nElem = 0;
}

// TODO: Este operador debe quedar fuera de la clase
// template <typename Traits>
// std::ostream &operator<<(std::ostream &os, CDoubleLinkedList<Traits> &obj){
//     auto pRoot = obj.GetRoot();
//     while( pRoot )
//         os << pRoot->GetData() << " ";
//     return os;
// }

void DemoDoubleLinkedList();

#endif // __DOUBLE_LINKEDLIST_H__
