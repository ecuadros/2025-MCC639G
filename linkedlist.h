#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <thread>
#include <mutex>
#include "types.h"
#include "traits.h"

template <typename Traits>
class LLNode{
private:
    using value_type = typename Traits::value_type;
    using Node = LLNode<Traits>;
    value_type     m_data;
    Ref            m_ref;
    Node          *m_pNext = nullptr;

public:
    LLNode(value_type &elem, Ref ref, Node *pNext = nullptr)
        : m_data(elem), m_ref(ref), m_pNext(pNext){
    }
    value_type   GetData()    { return m_data;     }
    value_type  &GetDataRef() { return m_data;     }
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
    Ref    GetRef()     { return m_ref;      }
};

template <typename Traits>
class CLinkedList{
private:
    using Type = typename Traits::value_type;
    using Node = LLNode<Traits>;
    Node *m_pRoot = nullptr;
    std::mutex m_mutex;
    size_t m_nElem = 0;

public:
    CLinkedList() {}  // Implementación inline
    
    CLinkedList(CLinkedList &other) {
        if (other.m_pRoot == nullptr){
            m_pRoot = nullptr;
            m_nElem = 0;
            return;
        }

        m_pRoot = new Node(other.m_pRoot->GetDataRef(), other.m_pRoot->GetRef());
        m_nElem = 1;

        Node* currentThis = m_pRoot;
        Node* currentOther = other.m_pRoot->GetNext();
        
        while (currentOther != nullptr) {
            currentThis->GetNextRef() = new Node(currentOther->GetDataRef(), currentOther->GetRef());
            currentThis = currentThis->GetNext();
            currentOther = currentOther->GetNext();
            m_nElem++;
        }
    }
    
    void Insert(Type &elem, Ref ref) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, elem, ref);
    }
    
    bool IsEmpty() const { return m_pRoot == nullptr; }
    size_t GetSize() const { return m_nElem; }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        UnsafeClear();
    }
    
    void Read(std::istream &is) {
        std::lock_guard<std::mutex> lock(m_mutex);
        UnsafeClear();
        
        Type elem;
        Ref ref;
        
        while (is >> elem >> ref) {
            InternalInsert(m_pRoot, elem, ref);
            if (is.fail()) break;
        }
    }
    
    Node *GetRoot() { return m_pRoot; }

private:
    void InternalInsert(Node *&rParent, Type &elem, Ref ref) {
        if( !rParent || elem < rParent->GetDataRef() ){
            rParent = new Node(elem, ref, rParent);
            m_nElem++;
            return;
        }
        InternalInsert(rParent->GetNextRef(), elem, ref);
    }
    
    void UnsafeClear() {
        while (m_pRoot != nullptr) {
            Node* temp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete temp;
        }
        m_pRoot = nullptr;
        m_nElem = 0;
    }
};

template <typename Traits>
std::ostream& operator<<(std::ostream &os, CLinkedList<Traits> &obj) {
    auto current = obj.GetRoot();
    while (current != nullptr) {
        os << current->GetData() << "(" << current->GetRef() << ") ";
        current = current->GetNext();
    }
    return os;
}

void DemoLinkedList();

#endif // __LINKEDLIST_H__