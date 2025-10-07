#ifndef DOUBLE_LINKED_LIST_TRAITS_H
#define DOUBLE_LINKED_LIST_TRAITS_H

#include <ostream>
#include <istream>
#include <utility>
#include "DoubleLinkedListTraitsNode.h"
#include "DoubleLinkedListTraitsIterator.h"

template <typename T>
class DoubleLinkedListTraits {
public:
    typedef DoubleLinkedListTraitsNode<T> node_type;
    typedef node_type* node_pointer;
    typedef typename node_type::value_type value_type;
    typedef DoubleLinkedListTraitsIterator<T> iterator;

    DoubleLinkedListTraits() : m_head(nullptr), m_tail(nullptr) {}

    ~DoubleLinkedListTraits() {
        clear();
    }

    void add(value_type info) {
        node_pointer new_node = new node_type(info);
        if (m_head == nullptr) {
            m_head = new_node;
            m_tail = new_node;
        } else {
            m_tail->set_next(new_node);
            new_node->set_prev(m_tail);
            m_tail = new_node;
        }
    }

    iterator begin() const { return iterator(m_head); }
    iterator end() const { return iterator(nullptr); }

    DoubleLinkedListTraits(const DoubleLinkedListTraits& other) : m_head(nullptr), m_tail(nullptr) {
        for (const auto& value : other) {
            add(value);
        }
    }

    DoubleLinkedListTraits& operator=(const DoubleLinkedListTraits& other) {
        if (this != &other) {
            clear();
            for (const auto& value : other) {
                add(value);
            }
        }
        return *this;
    }

    DoubleLinkedListTraits(DoubleLinkedListTraits&& other) noexcept 
        : m_head(other.m_head), m_tail(other.m_tail) {
        other.m_head = nullptr;
        other.m_tail = nullptr;
    }

    DoubleLinkedListTraits& operator=(DoubleLinkedListTraits&& other) noexcept {
        if (this != &other) {
            clear();
            m_head = other.m_head;
            m_tail = other.m_tail;
            other.m_head = nullptr;
            other.m_tail = nullptr;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const DoubleLinkedListTraits<T>& list) {
        for (const auto& value : list) {
            os << value << " ";
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, DoubleLinkedListTraits<T>& list) {
        value_type value;
        list.clear();
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

#endif // DOUBLE_LINKED_LIST_TRAITS_H
