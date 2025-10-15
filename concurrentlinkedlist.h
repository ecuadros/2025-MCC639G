#ifndef __CONCURRENTLINKEDLIST_H__
#define __CONCURRENTLINKEDLIST_H__
#include "types.h"
#include <mutex>
#include "linkedlist.h"
template <typename T> class CConcurrentLinkedList;

template <typename T>
std::ostream& operator<<(std::ostream& os, const CConcurrentLinkedList<T>& list);

/*
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
    

    
};
*/

template <typename T>
class CConcurrentLinkedList{
private:
    using Type = T; 
    using Node =  LLNode<Type>  ; 
    Node *m_pHead = nullptr;
    mutable std::mutex m_mutex;
public:
    // Constructor
    CConcurrentLinkedList();// : m_pHead(nullptr) {}
    // TODO: Constructor Copia DONE
    CConcurrentLinkedList(CConcurrentLinkedList &other);

    // TODO: Move contructor DONE
    CConcurrentLinkedList(CConcurrentLinkedList &&other);

    // Destructor seguro DONE
    virtual ~CConcurrentLinkedList();

    // concurrent 
    void Insert(Type &elem, Ref ref);
    
    friend std::ostream& operator<< <>(std::ostream& os, const CConcurrentLinkedList<T>& list);

    Node* GetHead(){ 
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pHead; 
        }
    
    void Print(std::ostream &os) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        os << "[";
        Node *pCurrent = m_pHead;
        while (pCurrent) {
            os << pCurrent->GetData();
            if (pCurrent->GetNext()) {
                os << ", ";
            }
            pCurrent = pCurrent->GetNext();
        }
        os << "]";
    }
private:
    // TODO: Implementar
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);
    
};

template <typename T>
std::ostream& operator<<(std::ostream &os, const CConcurrentLinkedList<T> &list) {
    list.Print(os);
    return os;
}

// copy constructor: DONE
template <typename T>
CConcurrentLinkedList<T>::CConcurrentLinkedList(CConcurrentLinkedList &other) : m_pHead(nullptr)
{
    std::lock_guard<std::mutex> guard(other.m_mutex);
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
void CConcurrentLinkedList<T>::Insert(Type &elem, Ref ref){
    InternalInsert(m_pHead, elem, ref);
}

template <typename T>
void CConcurrentLinkedList<T>::InternalInsert(Node *&rParent, Type &elem, Ref ref){
    if( !rParent || elem < rParent->GetDataRef() ){
        rParent = new Node(elem, ref, rParent);
        return;
    }
    // Tail recursion
    InternalInsert(rParent->GetNextRef(), elem, ref);
}

template <typename T>
CConcurrentLinkedList<T>::CConcurrentLinkedList()
{
}



// move constructor
template <typename T>
CConcurrentLinkedList<T>::CConcurrentLinkedList(CConcurrentLinkedList &&other)
{
    std::lock_guard<std::mutex> guard(other.m_mutex);
    m_pHead = other.m_pHead;
    other.m_pHead = nullptr;
}

// Destructor
template <typename T>
CConcurrentLinkedList<T>::~CConcurrentLinkedList()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    Node *current = m_pHead;
    while (current != nullptr){
        Node *next = current->GetNext();
        delete current;
        current = next;
    }
    m_pHead = nullptr;
}


void DemoConcurrentLinkedList();

#endif // __CONCURRENTLINKEDLIST_H__