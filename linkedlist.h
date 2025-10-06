#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include "types.h"

template <typename T, typename _Func>
struct LLinkedListTraits{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct LLinkedListAsc : 
    public LLinkedListTraits<T, std::less<T> >{
};

template <typename T>
struct LLinkedListDesc : 
    public LLinkedListTraits<T, std::greater<T> >{
};

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
// TODO Activar el iterator
template <typename Container> // HERE: TTraits -> Container
class forward_linkedlist_iterator{
 private:
     using value_type = typename Container::value_type;
     using Node       = typename Container::Node;
     using iterator   = forward_linkedlist_iterator<Container>;

     Container *m_pList = nullptr;
     Node      *m_pNode = nullptr;
 public:
     forward_linkedlist_iterator(Container *pList, Node *pNode)
             : m_pList(pList), m_pNode(pNode){}
     forward_linkedlist_iterator(iterator &other)
             : m_pList(other.m_pList), m_pNode(other.m_pNode){}   
     bool operator==(iterator other){ return m_pList == other.m_pList && m_pNode == other.m_pNode; }
     bool operator!=(iterator other){ return !(*this == other);    }

     iterator operator++(){ 
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
    using iterator   = forward_linkedlist_iterator<Container>;
    

private:
    Node   *m_pRoot = nullptr;
    size_t m_nElem = 0;
    Func   m_fCompare;
    mutable std::shared_mutex m_mutex;

public:
    // Constructor
    CLinkedList();
    CLinkedList(CLinkedList &other);
    // TODO: Move contructor (leer bibliografia)
    CLinkedList(CLinkedList &&other);

    // Destructor seguro
    virtual ~CLinkedList();

    void Insert(value_type &elem, Ref ref);
    void InsertThreadSafe(value_type &elem, Ref ref);
    void PrintThreadSafe() const;
    size_t SizeThreadSafe() const;
    bool FindThreadSafe(value_type &elem) const;
    void ReadLockedOperation(std::function<void()> operation) const;
    void WriteLockedOperation(std::function<void()> operation);

    Node *GetRoot()    {    return m_pRoot;     };
    iterator begin(){ return iterator(this, m_pRoot); };
    iterator end()  { return iterator(this, nullptr); } 


private:
    void InternalInsert(Node *&rParent, value_type &elem, Ref ref);
    //Node *GetRoot()    {    return m_pRoot;     };

public:


    friend std::ostream& operator<<(std::ostream &os, CLinkedList<Traits> &obj){
        auto pRoot = obj.GetRoot();
        while( pRoot ){
            os << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
            pRoot = pRoot->GetNext();
        }
        return os;
    }
};

template <typename Traits>
inline void CLinkedList<Traits>::InsertThreadSafe(value_type &elem, Ref ref) {
    std::unique_lock<std::shared_mutex> lock(m_mutex); // Lock exclusivo
    InternalInsert(m_pRoot, elem, ref);
}

template <typename Traits>
inline void CLinkedList<Traits>::PrintThreadSafe() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex); // Lock compartido
    auto pRoot = m_pRoot;
    while (pRoot) {
        std::cout << pRoot->GetData() << "(" << pRoot->GetRef() << ") ";
        pRoot = pRoot->GetNext();
    }
    std::cout << std::endl;
}

template <typename Traits>
inline size_t CLinkedList<Traits>::SizeThreadSafe() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex); // Lock compartido
    return m_nElem;
}

template <typename Traits>
inline bool CLinkedList<Traits>::FindThreadSafe(value_type &elem) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex); // Lock compartido
    auto current = m_pRoot;
    while (current) {
        if (current->GetData() == elem) {
            return true;
        }
        current = current->GetNext();
    }
    return false;
}

template <typename Traits>
inline void CLinkedList<Traits>::ReadLockedOperation(std::function<void()> operation) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    operation();
}

template <typename Traits>
inline void CLinkedList<Traits>::WriteLockedOperation(std::function<void()> operation) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    operation();
}

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

// TODO Constructor por copia 01
//      Hacer loop copiando cada elemento
// template <typename Traits>
// CLinkedList<Traits>::CLinkedList(CLinkedList &other){
// }

template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &other):m_pRoot(nullptr),m_nElem(0),m_fCompare(other.m_fCompare){
    //validamos si lista original esta vacía
    if(!other.w_pRoot){
        return;
    }

    Node* currentOther = other.m_pRoot;
    Node** currentThis = &m_pRoot;

    while (currentOther){

        //Crear una copia del nodo actual
        *currentThis = new Node(currentOther->GetDataRef(),currentOther->GetRef());

        m_nElem++;

        //Mover al siguiente nodo ambas listas
        currentThis = &((*currentThis)->GetNextRef());
        currentOther = currentOther->GetNext();


    }
}
//Destructor 02
template <typename Traits>
inline CLinkedList<Traits>::~CLinkedList() {
    Node* current = m_pRoot;
    while (current) {
        Node* next = current->GetNext();
        delete current;
        current = next;
    }
    m_pRoot = nullptr;
    m_nElem = 0;
}

// Move Constructor
template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &&other){
    m_pRoot    = std::move(other.m_pRoot);
    m_nElem    = std::move(other.m_nElem);
    m_fCompare = std::move(other.m_fCompare);
}

// template <typename Traits>
// CLinkedList<Traits>::~CLinkedList()
// {
// }

// TODO: Este operador debe quedar fuera de a clase
// template <typename Traits>
// std::ostream &operator<<(std::ostream &os, CLinkedList<Traits> &obj){
//     auto pRoot = obj.GetRoot();
//     while( pRoot )
//         os << pRoot->GetData() << " ";
//     return os;
// }

void DemoLinkedList();
void DemoConcurrencia();

#endif // __LINKEDLIST_H__