#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "types.h"
#include <mutex>

template <typename T>
class LLNode{
private:
    using    Type = T;
    using    Node = typename LLNode<T>;
    Type     m_data;
    Ref      m_ref;
    Node    *m_pNext = nullptr;
    std::mutex m;

public:
    LLNode(Type &elem, Ref ref, LLNode<T> *pNext = nullptr)
        : m_data(elem), m_pNext(pNext){
    }
    Type   GetData()    { return m_data;     }
    Type  &GetDataRef() { return m_data;     }
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
};

template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node = typename LLNode<Type>  ; 
    // Node<Type> *m_pHead = nullptr;
public:
    // Constructor
    CLinkedList();
    // TODO: Constructor Copia
    CLinkedList(CLinkedList &other){
        std::scoped_lock();
    };

    // TODO: Move contructor
    CLinkedList(CLinkedList &&other);

    // Destructor seguro
    virtual ~CLinkedList();

    void Insert(Type &elem, Ref ref);
private:
    // TODO: Implementar
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);
};

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
CLinkedList<T>::CLinkedList(CLinkedList &other)
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