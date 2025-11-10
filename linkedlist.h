#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "types.h"

using namespace std;

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
    Ref    GetRef()     { return m_ref;      }
};

template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node = LLNode<Type>;
    Node *m_pRoot = nullptr;
    std::mutex m_mutex;
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

    bool IsEmpty() const { return m_pRoot == nullptr; }

    // Nueva función: Leer desde stream
    void Read(std::istream &is);
    
    // Limpiar la lista
    void Clear();

    int GetSize() const {
        int count = 0;
        Node* current = m_pRoot;
        while (current) {
            count++;
            current = current->GetNext();
        }
        return count;
    }

    

private:
    // TODO: Implementar
    void InternalInsert(Node *&rParent, Type &elem, Ref ref);
    Node *GetRoot()    {    return m_pRoot;     };
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream &os, CLinkedList<U> &obj);

    template <typename U>
    friend std::istream& operator>>(std::istream &is, CLinkedList<U> &obj);
};

template <typename T>
void CLinkedList<T>::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    while (m_pRoot != nullptr) {
        Node* temp = m_pRoot;
        m_pRoot = m_pRoot->GetNext();
        delete temp;
    }
}


template <typename T>
void CLinkedList<T>::Read(std::istream &is) {
    std::lock_guard<std::mutex> lock(m_mutex);
       
    while (m_pRoot != nullptr) {
        Node* temp = m_pRoot;
        m_pRoot = m_pRoot->GetNext();
        delete temp;
    }

 
    Type elem;
    Ref ref;
    
    int count = 0;
    while (is >> elem >> ref) {
        cout << "Insertando " << elem << "(" << ref << ")" << endl;
        InternalInsert(m_pRoot, elem, ref);
        count++;
    }
    
    cout << "Read() completado. Pares leídos: " << count << endl;
}

// Sobrecarga del operador >> para facilitar la lectura
template <typename T>
std::istream& operator>>(std::istream &is, CLinkedList<T> &obj) {
    obj.Read(is);
    return is;
}


template <typename T>
void CLinkedList<T>::Insert(Type &elem, Ref ref){
    std::lock_guard<std::mutex> lock(m_mutex);
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
    if (other.m_pRoot == nullptr){
        m_pRoot = nullptr;
        return;
    }

    m_pRoot = new Node(other.m_pRoot->GetDataRef(),other.m_pRoot->GetRef());

    Node* currentThis = m_pRoot;
    Node* currentOther = other.m_pRoot->GetNext();
    
    while (currentOther != nullptr)
    {
        //crear un nuevo nodo con los mismos datos
        currentThis->GetNextRef() = new Node(currentOther->GetDataRef(), currentOther->GetRef());

        currentThis = currentThis->GetNext();
        currentOther = currentOther->GetNext();

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
        delete current; //liberar nodo actual
        current = next; //Avanzar al siguiente

    }

    m_pRoot = nullptr;
    
}

template <typename T>
std::ostream& operator<<(std::ostream &os, CLinkedList<T> &obj) {
    typename CLinkedList<T>::Node* current = obj.m_pRoot;
    while (current != nullptr) {
        os << current->GetData() << " ";
        current = current->GetNext();
    }
    return os;
}

void DemoLinkedList();
void SimpleConcurrencyTest();
void TestReadFunction();

#endif // __LINKEDLIST_H__