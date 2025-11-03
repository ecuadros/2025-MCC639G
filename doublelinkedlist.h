#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__
#include <iostream>
#include <shared_mutex>
#include <mutex>
#include "types.h"
#include "traits.h"

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

    // 
    Node * GetPrev()    { return m_pPrev;    }
    Node *&GetPrevRef() { return m_pPrev;    }
    // Diff
    void   SetPrev(Node *pPrev){    m_pPrev = pPrev; }
};

// 
// TODO Activar el forward_iterator
template <typename Container>
class forward_double_linkedlist_iterator{
using Node       = typename Container::Node;
    private:
    using value_type = typename Container::value_type;
// Diff
    using iterator   = forward_double_linkedlist_iterator<Container>;

    Container *m_pList = nullptr;
    Node      *m_pNode = nullptr;
public:
    forward_double_linkedlist_iterator(Container *pList, Node *pNode)
            : m_pList(pList), m_pNode(pNode){}
    forward_double_linkedlist_iterator(iterator &other)
            : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
    bool operator==(iterator other){ return m_pList == other.m_pList && m_pNode == other.m_pNode; }
    bool operator!=(iterator other){ return !(*this == other);    }

    // Diff
    iterator operator++(){ 
        if(m_pNode)
            m_pNode = m_pNode->GetNext();
        return *this;
    }
    value_type &operator*(){    return m_pNode->GetDataRef();   }
};

template <typename Container>
class backward_double_linkedlist_iterator{
 private:
     using value_type = typename Container::value_type;
     using Node       = typename Container::Node;
     // Diff
     using iterator   = backward_double_linkedlist_iterator<Container>;

     Container *m_pList = nullptr;
     Node      *m_pNode = nullptr;
 public:
     backward_double_linkedlist_iterator(Container *pList, Node *pNode)
             : m_pList(pList), m_pNode(pNode){}
     backward_double_linkedlist_iterator(iterator &other)
             : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
     bool operator==(iterator other){ return m_pList == other.m_pList && 
                                             m_pNode == other.m_pNode;
                                    }
     bool operator!=(iterator other){ return !(*this == other);    }

     // Diff
     iterator operator++(){ 
         if(m_pNode)
             m_pNode = m_pNode->GetPrev();
         return *this;
     }
     value_type &operator*(){    return m_pNode->GetDataRef();   }
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
    using forward_iterator   = forward_double_linkedlist_iterator<Container>;
    using backward_iterator  = backward_double_linkedlist_iterator<Container>;
    
private:
    Node   *m_pRoot = nullptr;
    Node   *m_pTail = nullptr;
    size_t m_nElem = 0;
    Func   m_fCompare;
    mutable std::shared_mutex m_mutex;

public:
    // Constructor
    CDoubleLinkedList();
    CDoubleLinkedList(CDoubleLinkedList &other);

    // TODO: Doneppppppppppppppooo55555555555555555555555
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
    // corregio backward iterator
    backward_iterator rbegin(){ return backward_iterator(this, m_pRoot); };
    backward_iterator rend()  { return backward_iterator(this, nullptr); } 

    friend std::ostream& operator<<(std::ostream &os, CDoubleLinkedList<Traits> &obj){
        std::shared_lock lock(obj.m_mutex);
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
        std::shared_lock lock(m_mutex);    
        return os << *this; 
    }
    
    // TODO: Read (istream &is)
    std::istream &Read (std::istream &is);
    friend std::istream &operator>>(std::istream &is, CDoubleLinkedList<Traits> &list) {
        return list.Read(is);
        }
};

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(value_type &elem, Ref ref){
    std::unique_lock lock(m_mutex);
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

template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(){}

// TODO Constructor por copia DONE
//      Hacer loop copiando cada elemento
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &other) 
    : m_pRoot(nullptr), m_pTail(nullptr), m_nElem(0), m_fCompare(other.m_fCompare)
{
    std::unique_lock lock(m_mutex);
    if (!other.m_pRoot) { // empty contdition
        return;
    }
    
    // copy first node
    m_pRoot = new Node(other.m_pRoot->GetDataRef(), other.m_pRoot->GetRef());
    m_pRoot->SetPrev(nullptr);
    
    Node *current = m_pRoot;
    Node *otherCurrent = other.m_pRoot->GetNext();
    
    // remaining nodes
    while (otherCurrent) {
        Node *newNode = new Node(otherCurrent->GetDataRef(), otherCurrent->GetRef());
        current->SetNext(newNode);
        newNode->SetPrev(current);
        
        current = newNode;
        otherCurrent = otherCurrent->GetNext();
    }
    
    m_pTail = current;
    m_nElem = other.m_nElem;
}


// Move Constructor
template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &&other){
    m_pRoot    = std::move(other.m_pRoot);
    m_nElem    = std::move(other.m_nElem);
    m_fCompare = std::move(other.m_fCompare);
    std::unique_lock lock(m_mutex);
}

// TODO: Implementar y liberar la memoria de cada Node. DONE
template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList()
{
    std::unique_lock lock(m_mutex);
    Node *current = m_pRoot;
    while (current) {
        Node *next = current->GetNext();
        delete current;
        current = next;
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;
    m_nElem = 0;
}

// Read implementation
template <typename Traits>
std::istream &CDoubleLinkedList<Traits>::Read(std::istream &is)
{
    //std::unique_lock lock(m_mutex);
    Node *current = m_pRoot;
    while (current) {
        Node *next = current->GetNext();
        delete current;
        current = next;
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;
    m_nElem = 0;
    char ch;
    value_type data;
    Ref ref;
    
    while (is >> std::ws && is.peek() != EOF){
        // Read the data value
        if (!(is >> data)){
            // Skip invalid characters and continue
            is.clear();
            is.ignore(1);
            continue;
        }
        
        // Read the opening parenthesis
        is >> std::ws >> ch;
        if (ch != '(') {
            // Skip this malformed entry
            is.ignore(256, ','); 
            continue;
        }
        
        // Read the ref value
        if (!(is >> ref)) {
            // Skip this malformed entry
            is.ignore(256, ','); 
            continue;
        }
        
        // Read the closing parenthesis
        is >> std::ws >> ch;
        if (ch != ')') {
            is.ignore(256, ','); 
            continue;
        }
        
        // Insert value and ref
        Insert(data, ref);
        
        // Skip whitespace and check for comma
        is >> std::ws;
        if (is.peek() == ',') {
            is.get(); 
        }
    }
    
    return is;
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

#endif // __DOUBLELINKEDLIST_H__