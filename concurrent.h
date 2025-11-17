#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

#include "linkedlist.h"   // Usa tu CLinkedList original

//
// Clase concurrente: envuelve tu lista normal con un mutex
//
template <typename Traits>
class CConcurrentLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Ref        = ::Ref;   
    using Node       = LLNode<Traits>;
    using iterator   = typename CLinkedList<Traits>::iterator;

private:
    CLinkedList<Traits> m_list;     // Tu lista normal
    mutable std::mutex  m_mutex;    // Mutex de control

public:
    CConcurrentLinkedList() = default;

    void Insert(value_type &elem, Ref ref)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_list.Insert(elem, ref);
    }

    iterator begin()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.begin();
    }

    iterator end()
    {
        return m_list.end();
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    CConcurrentLinkedList<Traits>& list)
    {
        std::lock_guard<std::mutex> lock(list.m_mutex);
        os << list.m_list;   
        return os;
    }
};


//
// Función de prueba de concurrencia
//
void TestConcurrency();