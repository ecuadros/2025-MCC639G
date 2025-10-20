#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "types.h"
#include "traits.h"

template <typename Traits>
class LLNode{
private:
    using    value_type = typename Traits::value_type;
    using    Node       = LLNode<Traits>;
    using    MySelf     = LLNode<Traits>;
    value_type          m_data;
    Ref      m_ref;
    Node    *m_pNext = nullptr;

public:
    LLNode(value_type &elem, Ref ref, LLNode<Traits> *pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext){
    }
    value_type   GetData()    { return m_data;     }
    value_type  &GetDataRef() { return m_data;     }
    Ref    GetRef()     { return m_ref;      }
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
};

// 
// TODO Activar el forward_iterator
template <typename Container> // HERE: TTraits -> Container
class forward_linkedlist_iterator{
 private:
     using value_type = typename Container::value_type;
     using Node       = typename Container::Node;
     using forward_iterator   = forward_linkedlist_iterator<Container>;

     Container *m_pList = nullptr;
     Node      *m_pNode = nullptr;
 public:
     forward_linkedlist_iterator(Container *pList, Node *pNode)
             : m_pList(pList), m_pNode(pNode){}
     forward_linkedlist_iterator(forward_iterator &other)
             : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
     bool operator==(forward_iterator other){ return m_pList == other.m_pList && m_pNode == other.m_pNode; }
     bool operator!=(forward_iterator other){ return !(*this == other);    }

     forward_iterator operator++(){ 
         if(m_pNode)
             m_pNode = m_pNode->GetNext();
         return *this;
     }
     value_type &operator*(){    return m_pNode->GetDataRef();   }
};

// TODO Agregar control de concurrencia

// TODO Agregar que sea ascendente o descendente con el mismo codigo
template <typename Traits>
class CLinkedList{
public:
    using value_type = typename Traits::value_type; 
    using Func       = typename Traits::Func;
    using Node       = LLNode<Traits>; 
    using Container  = CLinkedList<Traits>;
    using forward_iterator   = forward_linkedlist_iterator<Container>;

private:
    Node   *m_pRoot = nullptr;
    size_t m_nElem = 0;
    Func   m_fCompare;

public:
    // Constructor
    CLinkedList();
    CLinkedList(CLinkedList &other);
    CLinkedList(const CLinkedList &other);
    // TODO: Done
    CLinkedList(CLinkedList &&other);

    // Destructor seguro
    virtual ~CLinkedList();

    void Insert(value_type &elem, Ref ref);
private:
    void InternalInsert(Node *&rParent, value_type &elem, Ref ref);
    Node *GetRoot()    {    return m_pRoot;     };

public:
    forward_iterator begin(){ return forward_iterator(this, m_pRoot); };
    forward_iterator end()  { return forward_iterator(this, nullptr); } 

    CLinkedList<Traits>& operator=(const CLinkedList<Traits>& other);
    friend std::ostream& operator<<(std::ostream &os, CLinkedList<Traits> &obj){
        auto pRoot = obj.GetRoot();
        while( pRoot ){
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }
public:
    // Persistence
    std::ostream &Write(std::ostream &os) { return os << *this; }
    
    // TODO: Read (istream &is)
    std::istream &Read (std::istream &is);
};

template <typename Traits>
void CLinkedList<Traits>::Insert(value_type &elem, Ref ref){
    InternalInsert(m_pRoot, elem, ref);
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, value_type &elem, Ref ref){
    if( !rParent || m_fCompare(elem, rParent->GetDataRef()) ){
        rParent = new Node(elem, ref, rParent);
        m_nElem++;
        return;
    }
    // Tail recursion
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

template <typename Traits>
CLinkedList<Traits>::CLinkedList(){}

// TODO Constructor por copia
//      Hacer loop copiando cada elemento
template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &other)
                    : m_pRoot(nullptr), m_nElem(0), m_fCompare(other.m_fCompare){
    if (other.m_pRoot == nullptr){
        return; // nothing to copy
    }

    Node* pCurrentOther = other.m_pRoot;
    Node* pLastNew = nullptr;

    // initialize: 1st nodo
    m_pRoot = new Node(pCurrentOther->GetData(), pCurrentOther->GetRef());
    pLastNew = m_pRoot;
    m_nElem++;
    pCurrentOther = pCurrentOther->GetNext();
    // loop on the netxt
    while (pCurrentOther != nullptr){
        Node* pNewNode = new Node(pCurrentOther->GetData(), pCurrentOther->GetRef());
        pLastNew->GetNextRef() = pNewNode;
        pLastNew = pNewNode;
        m_nElem++;
        pCurrentOther = pCurrentOther->GetNext();
    }

}

// Move Constructor
template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &&other){
    m_pRoot    = std::move(other.m_pRoot);
    m_nElem    = std::move(other.m_nElem);
    m_fCompare = std::move(other.m_fCompare);
}

// TODO: Implementar y liberar la memoria de cada Node
template <typename Traits>
CLinkedList<Traits>::~CLinkedList()
{
}

// TODO: Este operador debe quedar fuera de la clase
template <typename Traits>
std::ostream &operator<<(std::ostream &os, CLinkedList<Traits> &obj){
    auto pRoot = obj.GetRoot();
    while( pRoot )
        os << pRoot->GetData() << " ";
    return os;
}

// operador de asignacion
template <typename Traits>
CLinkedList<Traits>& CLinkedList<Traits>::operator=(const CLinkedList &other) {
    if (this != &other) {  // previene autoasignacion
        
        while (m_pRoot != nullptr) {
            Node* pTemp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete pTemp;
        }
        m_nElem = 0;
        
        // copiar desde other / contr x copia
        if (other.m_pRoot != nullptr) {
            Node* pCurrentOther = other.m_pRoot;
            Node* pLastNew = nullptr;
            
            m_pRoot = new Node(pCurrentOther->GetDataRef(), pCurrentOther->GetRef());
            pLastNew = m_pRoot;
            m_nElem++;
            
            pCurrentOther = pCurrentOther->GetNext();
            
            while (pCurrentOther != nullptr) {
                Node* pNewNode = new Node(pCurrentOther->GetDataRef(), pCurrentOther->GetRef());
                pLastNew->GetNextRef() = pNewNode;
                pLastNew = pNewNode;
                m_nElem++;
                pCurrentOther = pCurrentOther->GetNext();
            }
        }
        m_fCompare = other.m_fCompare;
    }
    return *this;  
}


void DemoLinkedList();

#endif // __LINKEDLIST_H__