#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "types.h"

template <typename T> class CLinkedList;

template <typename T>
std::ostream& operator<<(std::ostream& os, const CLinkedList<T>& list);

template <typename T>
class LLNode{
private:
    using    Type = T;
    using    Node = LLNode<T>;
    Type     m_data;
    Ref      m_ref;
    Node    *m_pNext = nullptr;

public:
    LLNode(Type &elem, Ref ref, LLNode<T> *pNext = nullptr)
        : m_data(elem), m_pNext(pNext){
    }
    Type   GetData()    { return m_data;     }
    Type  &GetDataRef() { return m_data;     }
    LLNode * GetNext()    { return m_pNext;    }
    LLNode *&GetNextRef() { return m_pNext;    }
    Ref GetRef(){ return m_ref;}
    
    friend std::ostream& operator<< (std::ostream &os, CLinkedList<T> &list);
    
};



template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node =  LLNode<Type>  ; 
    Node *m_pRoot = nullptr;
public:
    
    // Constructor
    CLinkedList();
    // TODO: Constructor Copia
    CLinkedList(CLinkedList &other);

    // TODO: Move contructor
    CLinkedList(CLinkedList &&other);

    // Destructor seguro
    virtual ~CLinkedList();

    void Insert(Type &elem, Ref ref);
private:
    // TODO: Implementar
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);
    Node GetRoot(){ return m_pRoot };
};

// copy constructor
template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &other) : m_pHead(nullptr)
{
    if (!other.m_pHead) return; // empty list
    Node *pOtherCurrent = other.m_pHead;
    m_pHead = new Node(pOtherCurrent->GetDataRef(), pOtherCurrent->GetRef());
    
    Node *pCurrent = m_pHead;
    pOtherCurrent = pOtherCurrent->GetNext();
    
    // Copiar los nodos restantes
    while (pOtherCurrent) {
        pCurrent->GetNextRef() = new Node(pOtherCurrent->GetDataRef(), 
                                        pOtherCurrent->GetRef());
        pCurrent = pCurrent->GetNext();
        pOtherCurrent = pOtherCurrent->GetNext();
    }
}

template <typename T>
std::ostream& operator<< <>(std::ostream &os, CLinkedList<T> &list){
        os << "[";
        typename CLinkedList<T>::Node *pCurrent = list.GetRoot();
        
        while (pCurrent) {
            os << pCurrent->GetData();
            if (pCurrent->GetNext()) {
                os << " -> ";
            }
            pCurrent = pCurrent->GetNext();
        }
        os << "]";
        return os;
    }


template <typename T>
void CLinkedList<T>::Insert(Type &elem, Ref ref){
    InternalInsert(m_pHead, elem, ref);
}

template <typename T>
void CLinkedList<T>::InternalInsert(Node *&rParent, Type &elem, Ref ref){
    if( !rParent || elem < rParent->GetDataRef() ){
        rParent = new Node(elem, ref, rParent);
        return;
    }
    // Tail recursion
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

template <typename T>
CLinkedList<T>::CLinkedList()
{
}



template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &&other)
{
}

template <typename T>
CLinkedList<T>::~CLinkedList()
{
}


void DemoLinkedList();

#endif // __LINKEDLIST_H__