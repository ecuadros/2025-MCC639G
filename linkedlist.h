#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "types.h"
#include <mutex>
#include <utility>

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
        : m_data(elem), m_ref(ref), m_pNext(pNext){
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
    using Node = typename LLNode<Type>;
    Node *m_pHead = nullptr;
    std::mutex m_listMutex;
public:
    // Constructor
    CLinkedList(): m_pHead(nullptr){};

    // TODO: Constructor Copia
    CLinkedList(const CLinkedList &other) : m_pHead(nullptr){
        std::scoped_lock lock(other.m_listMutex);

        if (other.m_pHead == nullptr) return;
        m_pHead = new Node(other.m_pHead->GetDataRef(), other.m_pHead->GetRef());
        Node* currentOther = other.m_pHead->GetNext();
        Node* currentThis = m_pHead;

        while (currentOther != nullptr) {
            currentThis->GetNextRef() = new Node(currentOther->GetDataRef(), 
                                                 currentOther->GetRef());
            currentThis = currentThis->GetNext();
            currentOther = currentOther->GetNext();
        }
        

    };

    // TODO: Move contructor
    CLinkedList(CLinkedList &&other){
        std::scoped_lock lock(m_listMutex,other.m_listMutex);
        m_pHead = std::exchange(other.m_pHead, nullptr);
    };


    // Destructor seguro
    virtual ~CLinkedList(){
        std::scoped_lock lock(m_listMutex);
        Node* head = std::exchange(m_pHead, nullptr);
        while (head != nullptr) {
            Node* temp = std::exchange(head, head->GetNext());
            delete temp;
        }
    };

    void Insert(Type &elem, Ref ref){
        std::scoped_lock lock(m_listMutex);
        InternalInsert(m_pHead, elem, ref);
    };

    // TODO: Implementar

    void InternalInsert(Node *&rParent, Type &elem, Ref ref){
        if (!rParent || ref < rParent->GetRef()){
            rParent = new Node(elem, ref, rParent);
            return;
        }
        InternalInsert(rParent->GetNextRef(), elem, ref);
    };

        void Print() const {
        std::scoped_lock lock(m_listMutex);
        
        Node* current = m_pHead;
        while (current != nullptr) {
            
            std::cout << current->GetData() << " ";
            current = current->GetNext();
        }
        std::cout << std::endl;
    }
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