#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "types.h"

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
    Node * GetNext()    { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
};

template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node = LLNode<Type>; 
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
    Node *GetRoot()    {    return m_pRoot;     };
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream &os, CLinkedList<U> &obj);
};

template <typename T>
void CLinkedList<T>::Insert(Type &elem, Ref ref){
    InternalInsert(m_pRoot, elem, ref);
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
//Constructor Copia
template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &other)
{
    Node* currentOther = other.m_pRoot;
    Node** currentThis = &m_pRoot;

    while (currentOther != nullptr)
    {
        //crear un nuevo nodo con los mismos datos
        *currentThis = new Node(currentOther->GetDataRef(), currentOther->GetRef());

        //avanzar ambas listas
        currentOther = currentOther->GetNext();
        currentThis = &((*currentThis)->GetNextRef());
    }
    
}

//Move contructor
template <typename T>
CLinkedList<T>::CLinkedList(CLinkedList &&other)
{
    //Move Contructor "quita" los nodos de 'other'
    m_pRoot = other.m_pRoot; //Tomamos ownership de los nodos
    other.m_pRoot = nullptr; //'other' queda vacío y seguro

}

template <typename T>
CLinkedList<T>::~CLinkedList()
{
    //Destructor libera la memoria
    Node* current = m_pRoot;
    while (current != nullptr)
    {
        /* code */
        Node* next = current->GetNext(); //Guardar referencia al siguiente
        delete current; //liberar nod actual
        current = next; //Avanzar al siguiente

    }

    m_pRoot = nullptr;
    
}

template <typename T>
std::ostream &operator<<(std::ostream &os, CLinkedList<T> &obj){
    auto pRoot = obj.GetRoot();
    while( pRoot ){
        os << pRoot->GetData() << " ";
        pRoot = pRoot->GetNext();
    }
        
    return os;
}

void DemoLinkedList();

#endif // __LINKEDLIST_H__