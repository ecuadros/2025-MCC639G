#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "types.h"
#include <mutex>
#include <iostream> // For ostream, istream
#include <utility>  // For std::swap 

// Nodo de la Lista Enlazada
template <typename T>
class LLNode{
private:
    using    Type = T;
    using    Node = typename LLNode<T>; // Tipo del nodo
    Type     m_data; // Dato almacenado
    Ref      m_ref; // Referencia asociada 
    Node    *m_pNext = nullptr; // Puntero al siguiente nodo en la lista

public:
    // Constructor
    LLNode(Type data, Ref ref, Node*pNext = nullptr)
        : m_data(std::move(data)), m_ref(ref), m_pNext(pNext){
    }
    Type   GetData()  const  { return m_data;     }
    Type  &GetDataRef() { return m_data;     }
    Node * GetNext()  const  { return m_pNext;    }
    Node *&GetNextRef() { return m_pNext;    }
    Ref GetRef() const { return m_ref; }
};

// Lista Enlazada Principal
template <typename T>
class CLinkedList{
private:
    using Type = T; 
    using Node = LLNode<Type>  ; 
    Node*m_pHead = nullptr; // puntero al primer nodo de la lista
    size_t m_size = 0;          // contador de elementos en la lista
    std::mutex m_mutex; // mutex para protección thread-safe

    // Inserción recursiva interna
    void InternalInsert(LLNode<T>*& rParent, T elem, Ref ref) {
        if (!rParent || elem < rParent->GetData()) {
            rParent = new LLNode<T>(std::move(elem), ref, rParent);
            m_size++;
            return;
        }
        InternalInsert(rParent->GetNextRef(), std::move(elem), ref);
    }
    
    // Limpieza interna
    void InternalClear() {
        while (m_pHead) {
            auto temp = m_pHead;
            m_pHead = m_pHead->GetNext();
            delete temp;
        }
        m_size = 0;
    }

public:
    // Constructor
    CLinkedList()= default;
    // TODO: Constructor Copia
    CLinkedList(CLinkedList &other) {
        std::scoped_lock lock(m_mutex, other.m_mutex); //mmutex
        for (auto& item : other) {
            Insert(item, Ref{}); // Ref podría necesitar ajuste
        }
    }

    // TODO: Move contructor
    CLinkedList(CLinkedList &&other)  {
        std::scoped_lock lock(other.m_mutex);
        m_pHead = std::exchange(other.m_pHead, nullptr); //usar exchange
        m_size = std::exchange(other.m_size, 0);

        //m_size = other.m_size; std::exchange(other m_size,0)
        //other.m_pHead = nullptr;
        //other.m_size = 0;
    }

    // Destructor seguro
    virtual ~CLinkedList() {
        std::scoped_lock lock(m_mutex);
        InternalClear();
    }


    // ============ OPERACIONES BÁSICAS ============
    // Inserta un elemento manteniendo orden ascendente
    void Insert(Type &elem, Ref ref) {
        std::scoped_lock lock(m_mutex);
        InternalInsert(m_pHead, std::move(elem), ref);
    }

    // Elimina todos los elementos de la lista
    void Clear() {
        std::scoped_lock lock(m_mutex);
        InternalClear();
    }

    // Búsqueda simple
    // Tomar por const&, ya que no modificamos elem
    // El método es const ya que no modifica la lista
    bool Contains(const T& elem) const {
        std::scoped_lock lock(m_mutex);
        auto current = m_pHead;
        while (current) {
            if (current->GetData() == elem) return true;
            current = current->GetNext();
        }
        return false;
    }
    
    // Eliminación práctica
    bool Remove(const T& elem) {
        std::scoped_lock lock(m_mutex);
        auto current = &m_pHead;
        
        while (*current) {
            if ((*current)->GetData() == elem) {
                auto temp = *current;
                *current = (*current)->GetNext();
                delete temp;
                m_size--;
                return true;
            }
            current = &(*current)->GetNextRef();
        }
        return false;
    }

    // 
    size_t Size() const { 
        std::scoped_lock lock(m_mutex);  // Thread-safe
        return m_size; 
    }

    bool Empty() const {
        std::scoped_lock lock(m_mutex);
        return m_size == 0;
    }

// ============ ITERADOR SIMPLE ============
    // ADVERTENCIA: La iteración no es thread-safe.
    // Si itera mientras otra hebra modifica la lista,
    // el comportamiento es indefinido.
    // Para iterar de forma segura, debe bloquear manualmente la lista
    // externamente durante *toda* la iteración.
    class Iterator {
    private:
        LLNode<T>* m_pCurrent;
    public:
        Iterator(LLNode<T>* pNode = nullptr) : m_pCurrent(pNode) {}
        
        T& operator*() { return m_pCurrent->GetDataRef(); }
        T* operator->() { return &m_pCurrent->GetDataRef(); }
        
        Iterator& operator++() {
            m_pCurrent = m_pCurrent->GetNext();
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) { 
            return m_pCurrent == other.m_pCurrent; 
        }
        
        bool operator!=(const Iterator& other) { 
            return m_pCurrent != other.m_pCurrent; 
        }
    };
    
    Iterator begin() { 
        std::scoped_lock lock(m_mutex);
        return Iterator(m_pHead); 
    }
    
    Iterator end() { return Iterator(nullptr); }
    
    // Para range-based for loops
    Iterator begin() { return Begin(); }
    Iterator end() { return End(); }
    
    // ============ SERIALIZACIÓN DIRECTA ============
    void Write(std::ostream &os) {
        std::scoped_lock lock(m_mutex);
        os << m_size << "\n";
        
        auto current = m_pHead;
        while (current) {
            os << current->GetData() << " ";
            os.write(reinterpret_cast<char*>(&current->GetRef()), sizeof(Ref));
            current = current->GetNext();
        }
    }
    
    void Read(std::istream &is) {
        std::scoped_lock lock(m_mutex);
        InternalClear();
        
        size_t elementCount;
        is >> elementCount;
        
        for (size_t i = 0; i < elementCount; ++i) {
            T data;
            Ref ref;
            is >> data;
            is.read(reinterpret_cast<char*>(&ref), sizeof(Ref));
            Insert(std::move(data), ref);
        }
    }
    
    // ============ OPERADORES ÚTILES ============
        // Asignación por copia-y-swap
    // 'other' se pasa por valor, creando una copia (usando Cpy-Ctor)
    // o moviendo (usando Move-Ctor) el argumento.
    CLinkedList& operator=(CLinkedList other) {
        swap(other);
        return *this;
    }
   } // 'other' (que ahora contiene los datos antiguos de 'this') se destruye   
    void swap(CLinkedList& other) {
        std::scoped_lock lock(m_mutex, other.m_mutex);
        std::swap(m_pHead, other.m_pHead);
        std::swap(m_size, other.m_size);
    }
    
    // Acceso rápido al primer elemento (si existe)
    T* Front() {
        std::scoped_lock lock(m_mutex);
        return m_pHead ? &m_pHead->GetData() : nullptr;
    }
};

// Función swap global
template<typename T>
void swap(CLinkedList<T>& lhs, CLinkedList<T>& rhs) {
    lhs.swap(rhs);
}

#endif // __LINKEDLIST_H__

