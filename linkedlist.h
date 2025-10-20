#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "types.h"
#include <sstream>

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
    

    
};



template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node =  LLNode<Type>  ; 
    Node *m_pHead = nullptr;
public:
    
    // Constructor
    CLinkedList();// : m_pHead(nullptr) {}
    // TODO: Constructor Copia DONE
    CLinkedList(CLinkedList &other);

    // TODO: Move contructor
    CLinkedList(CLinkedList &&other);

    // Destructor seguro DONE
    virtual ~CLinkedList();

    // concurrent 
    void Insert(Type &elem, Ref ref);
    friend std::ostream& operator<< <T>(std::ostream& os, const CLinkedList<T>& list);
    std::istream &Read(std::istream &is);
    Node* GetHead(){ return m_pHead; }
    friend std::istream &operator>>(std::istream &is, CLinkedList &list) {
        return list.Read(is);
    }
private:
    // TODO: Implementar
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);
    
};


// ---- Implementaciones ----
template <typename T>
std::ostream& operator<< (std::ostream &os, CLinkedList<T> &list){
        
        //typename CLinkedList<T>::Node *pCurrent = list.GetHead();
        auto *pCurrent = list.GetHead();//list.GetRoot();
        while (pCurrent) {
            os << pCurrent->GetData()<< "(" << pCurrent->GetRef() << ")";

            pCurrent = pCurrent->GetNext();
        }
        
        return os;
    }


template <typename Traits>
std::istream &CLinkedList<Traits>::Read(std::istream &is)
{
    // Clear existing list
    Node *current = m_pHead;
    while (current) {
        Node *next = current->GetNext();
        delete current;
        current = next;
    }
    m_pHead = nullptr;
    m_pTail = nullptr;
    m_nElem = 0;

    std::string line;
    // entire line
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string token;
        
        // data(ref) pair separated by space
        while (std::getline(ss, token, ' ')) {
            
            size_t openParen = token.find('(');
            size_t closeParen = token.find(')');
            
            if (openParen != std::string::npos && closeParen != std::string::npos && 
                openParen < closeParen) {
                std::string dataStr = token.substr(0, openParen);
                value_type data;
                std::stringstream dataStream(dataStr);
                dataStream >> data;
                
                std::string refStr = token.substr(openParen + 1, closeParen - openParen - 1);
                Ref ref;
                std::stringstream refStream(refStr);
                refStream >> ref;
                
                // insert into he list
                Insert(data, ref);
                
            }
        }
    }
    
    return is;
}

// copy constructor: DONE
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
    Node *current = m_pHead;
    while (current != nullptr){
        Node *next = current->GetNext();
        delete current;
        current = next;
    }
    m_pHead = nullptr;
}


void DemoLinkedList();

#endif // __LINKEDLIST_H__