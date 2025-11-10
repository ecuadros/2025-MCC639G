#ifndef __DOUBLE_LINKEDLIST_H__
#define __DOUBLE_LINKEDLIST_H__
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "types.h"
#include "traits.h"
#include "foreach.h"

template <typename Traits>
class DLLNode{
private:
    using    value_type = typename Traits::value_type;
    using    Node       = DLLNode<Traits>;

    value_type          m_data;
    Ref                 m_ref;
    Node               *m_pNext = nullptr;
    Node               *m_pPrev = nullptr;

public:
    DLLNode(value_type &elem, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext), m_pPrev(pPrev){
    }
    value_type   GetData()    { return m_data;     }
    value_type  &GetDataRef() { return m_data;     }
    Ref    GetRef()     { return m_ref;      }
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }

    void   SetNext(Node *pNext){    m_pNext = pNext; }

    Node * GetPrev()    { return m_pPrev;    }
    Node *&GetPrevRef() { return m_pPrev;    }

    void   SetPrev(Node *pPrev){    m_pPrev = pPrev; }
};

template <typename Container>
class forward_double_linkedlist_iterator{
 private:
     using value_type = typename Container::value_type;
     using Node       = typename Container::Node;
     using iterator   = forward_double_linkedlist_iterator<Container>;

     Container *m_pList = nullptr;
     Node      *m_pNode = nullptr;
 public:
     forward_double_linkedlist_iterator(Container *pList, Node *pNode)
             : m_pList(pList), m_pNode(pNode){}
     forward_double_linkedlist_iterator(const iterator &other)
             : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
     bool operator==(const iterator &other) const { 
         return m_pList == other.m_pList && m_pNode == other.m_pNode; 
     }
     bool operator!=(const iterator &other) const { 
         return !(*this == other);    
     }

     iterator &operator++(){ 
         if(m_pNode)
             m_pNode = m_pNode->GetNext();
         return *this;
     }
     
     iterator operator++(int){ 
         iterator temp = *this;
         ++(*this);
         return temp;
     }
     
     value_type &operator*(){ return m_pNode->GetDataRef(); }
     const value_type &operator*() const { return m_pNode->GetDataRef(); }
};

template <typename Container>
class backward_double_linkedlist_iterator{
 private:
     using value_type = typename Container::value_type;
     using Node = typename Container::Node;
     using iterator = backward_double_linkedlist_iterator<Container>;

     Container *m_pList = nullptr;
     Node      *m_pNode = nullptr;
 public:
     backward_double_linkedlist_iterator(Container *pList, Node *pNode)
             : m_pList(pList), m_pNode(pNode){}
     backward_double_linkedlist_iterator(const iterator &other)
             : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
     
     bool operator==(const iterator &other) const { 
         return m_pList == other.m_pList && m_pNode == other.m_pNode;
     }
     
     bool operator!=(const iterator &other) const { 
         return !(*this == other);    
     }

     // CORRECCIÓN: operator++ debe ir hacia atrás (prev)
     iterator &operator++(){ 
         if(m_pNode) {
             m_pNode = m_pNode->GetPrev();  // Ir al nodo anterior
         }
         return *this;
     }
     
     iterator operator++(int){ 
         iterator temp = *this;
         ++(*this);
         return temp;
     }
     
     value_type &operator*(){ 
         if (!m_pNode) {
             throw std::runtime_error("Dereferencing null iterator");
         }
         return m_pNode->GetDataRef(); 
     }
     
     const value_type &operator*() const { 
         if (!m_pNode) {
             throw std::runtime_error("Dereferencing null iterator");
         }
         return m_pNode->GetDataRef(); 
     }
     
     // Método para verificar si es válido
     bool isValid() const { return m_pNode != nullptr; }
};

template <typename Traits>
class CDoubleLinkedList{
public:
    using value_type = typename Traits::value_type; 
    using Func       = typename Traits::Func;
    using Node       = DLLNode<Traits>; 
    using Container  = CDoubleLinkedList<Traits>;
    using forward_iterator   = forward_double_linkedlist_iterator<Container>;
    using backward_iterator  = backward_double_linkedlist_iterator<Container>;
    
private:
    Node   *m_pRoot = nullptr;
    Node   *m_pTail = nullptr;
    size_t m_nElem = 0;
    Func   m_fCompare;
    std::mutex m_mutex;
    Node *GetTail() { return m_pTail; }

public:
    // Constructores
    CDoubleLinkedList();
    CDoubleLinkedList(CDoubleLinkedList &other);
    CDoubleLinkedList(CDoubleLinkedList &&other);

    // Destructor seguro
    virtual ~CDoubleLinkedList();

    // Operaciones básicas
    void Insert(value_type &elem, Ref ref);
    bool IsEmpty() const { return m_pRoot == nullptr; }
    size_t Size() const { return m_nElem; }
    void Clear();

    // Iteradores
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); };
    forward_iterator end()  { return forward_iterator(this, nullptr); } 

    backward_iterator rbegin(){ return backward_iterator(this, m_pTail); };
    backward_iterator rend()  { return backward_iterator(this, nullptr); } 

    // Operadores de stream
    friend std::ostream& operator<<(std::ostream &os, CDoubleLinkedList<Traits> &obj){
        auto pRoot = obj.GetRoot();
        while( pRoot ){
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }

    std::ostream &Write(std::ostream &os) { return os << *this; }
    std::istream &Read(std::istream &is);

private:
    void InternalInsert(Node *&rParent, value_type &elem, Ref ref);
    Node *GetRoot() { return m_pRoot; };
    void UnsafeClear(); // Versión sin mutex para uso interno
};

// Implementación del destructor
template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList()
{
    Clear();
}

// Implementación de Clear
template <typename Traits>
void CDoubleLinkedList<Traits>::UnsafeClear() {
    while (m_pRoot != nullptr) {
        Node* temp = m_pRoot;
        m_pRoot = m_pRoot->GetNext();
        delete temp;
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;
    m_nElem = 0;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    UnsafeClear();
}

// Implementación de Read
template <typename Traits>
std::istream &CDoubleLinkedList<Traits>::Read(std::istream &is) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Limpiar lista existente
    UnsafeClear();
    
    value_type elem;
    Ref ref;
    
    while (is >> elem >> ref) {
        InternalInsert(m_pRoot, elem, ref);
        if (is.fail()) break;
    }
    
    return is;
}

//constructor por copia
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &other) {
    std::lock_guard<std::mutex> lock(other.m_mutex);
    
    if (other.m_pRoot == nullptr) {
        m_pRoot = nullptr;
        m_pTail = nullptr;
        m_nElem = 0;
        return;
    }

    // Copiar primer nodo
    m_pRoot = new Node(other.m_pRoot->GetDataRef(), other.m_pRoot->GetRef());
    m_pRoot->SetPrev(nullptr);
    m_nElem = 1;

    Node* currentThis = m_pRoot;
    Node* currentOther = other.m_pRoot->GetNext();
    
    // Copiar nodos restantes manteniendo referencias dobles
    while (currentOther != nullptr) {
        Node* newNode = new Node(currentOther->GetDataRef(), currentOther->GetRef());
        
        // Configurar referencias dobles correctamente
        currentThis->SetNext(newNode);
        newNode->SetPrev(currentThis);
        
        // Avanzar al siguiente nodo
        currentThis = newNode;
        currentOther = currentOther->GetNext();
        m_nElem++;
    }
    
    // Configurar tail correctamente
    m_pTail = currentThis;
}
// Move Constructor
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &&other) {
    std::lock_guard<std::mutex> lock(other.m_mutex);
    
    m_pRoot = other.m_pRoot;
    m_pTail = other.m_pTail;
    m_nElem = other.m_nElem;
    m_fCompare = std::move(other.m_fCompare);
    
    other.m_pRoot = nullptr;
    other.m_pTail = nullptr;
    other.m_nElem = 0;
}

// Constructor por defecto
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(){}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(value_type &elem, Ref ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    InternalInsert(m_pRoot, elem, ref);
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InternalInsert(Node *&rParent, value_type &elem, Ref ref){
    if( !rParent || m_fCompare(elem, rParent->GetDataRef()) ){
        // Crear nuevo nodo
        Node *pNew = new Node(elem, ref, rParent);
        
        // Configurar referencia previa del nuevo nodo
        if (rParent) {
            // Si estamos insertando antes de un nodo existente
            pNew->SetPrev(rParent->GetPrev());
            rParent->SetPrev(pNew);
        } else {
            // Si es el último nodo (rParent es nullptr)
            pNew->SetPrev(m_pTail);
            m_pTail = pNew;
        }
        
        // Actualizar referencia del nodo anterior si existe
        if (pNew->GetPrev()) {
            pNew->GetPrev()->SetNext(pNew);
        }
        
        rParent = pNew;
        
        // Si es el primer nodo, actualizar m_pRoot
        if (!pNew->GetPrev()) {
            m_pRoot = pNew;
        }
        
        m_nElem++;
        return;
    }
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

// Tests
void DemoDoubleLinkedList();
void TestDoubleLinkedListConcurrency();
void TestDoubleLinkedListRead();
void TestDoubleLinkedListDestructor();

#endif // __DOUBLE_LINKEDLIST_H__