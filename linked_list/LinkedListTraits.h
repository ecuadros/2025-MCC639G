#ifndef LINKED_LIST_TRAITS_H
#define LINKED_LIST_TRAITS_H

#include <ostream>
#include <istream>
#include <utility> // For std::move
#include "LinkedListTraitsNode.h"
#include "LinkedListTraitsIterator.h"

template <typename T>
class LinkedListTraits {
public:
    typedef LinkedListTraitsNode<T> node_type;
    typedef node_type* node_pointer;

    typedef typename node_type::value_type value_type;
    typedef typename node_type::reference_type reference_type;
    typedef typename node_type::const_reference_type const_reference_type;

    typedef LinkedListTraitsIterator<T> iterator;

    LinkedListTraits() : m_head(nullptr), m_tail(nullptr) {}

    LinkedListTraits(const LinkedListTraits& other) : m_head(nullptr), m_tail(nullptr) {
        for (const auto& value : other) {
            add(value);
        }
    }

    LinkedListTraits& operator=(const LinkedListTraits& other) {
        if (this != &other) {
            clear();
            for (const auto& value : other) {
                add(value);
            }
        }
        return *this;
    }

    LinkedListTraits(LinkedListTraits&& other) noexcept : m_head(other.m_head), m_tail(other.m_tail) {
        other.m_head = nullptr;
        other.m_tail = nullptr;
    }

    LinkedListTraits& operator=(LinkedListTraits&& other) noexcept {
        if (this != &other) {
            clear(); // Free existing resources
            m_head = other.m_head;
            m_tail = other.m_tail;
            other.m_head = nullptr;
            other.m_tail = nullptr;
        }
        return *this;
    }

    ~LinkedListTraits() {
        clear();
    }

    void add(value_type info) {
        if (m_head == nullptr) {
            m_head = new node_type(info);
            m_tail = m_head;
        } else {
            m_tail->set_next(new node_type(info));
            m_tail = m_tail->next();
        }
    }

    iterator begin() const {
        return iterator(m_head);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    friend std::ostream& operator<<(std::ostream& os, const LinkedListTraits<T>& list) {
        for (const auto& value : list) {
            os << value << " ";
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, LinkedListTraits<T>& list) {
        value_type value;
        while (is >> value) {
            list.add(value);
        }
        return is;
    }

private:
    void clear() {
        for (node_pointer it = m_head; it != nullptr;) {
            node_pointer next = it->next();
            delete it;
            it = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
    }

    node_pointer m_head;
    node_pointer m_tail;
};

#endif // LINKED_LIST_TRAITS_H
